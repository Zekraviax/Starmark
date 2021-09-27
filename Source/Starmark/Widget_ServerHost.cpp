#include "Widget_ServerHost.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerController_Base.h"
#include "SaveData_PlayerProfilesList.h"
#include "Starmark_GameInstance.h"
#include "Starmark_PlayerState.h"
#include "WidgetComponent_PlayerProfile.h"
#include "Widget_MainMenu.h"


// ------------------------- Widget
void UWidget_ServerHost::OnWidgetOpened()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstanceReference->LoadProfile(GameInstanceReference->CurrentProfileName);
	Cast<AStarmark_PlayerState>(GetOwningPlayerState())->UpdatePlayerData();
}	

void UWidget_ServerHost::OnExitButtonPressed()
{
	if (!MainMenuWidget_Reference && MainMenuWidget_Class)
		MainMenuWidget_Reference = CreateWidget<UWidget_MainMenu>(this, MainMenuWidget_Class);

	if (MainMenuWidget_Reference) {
		MainMenuWidget_Reference->AddToViewport();
		this->RemoveFromParent();
	}
}


// ------------------------- Player
void UWidget_ServerHost::GetAllPlayerProfiles()
{
	// Load profiles
	USaveData_PlayerProfilesList* ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

	if (!ProfilesList->IsValidLowLevel()) {
		ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::CreateSaveGameObject(USaveData_PlayerProfilesList::StaticClass()));
		UGameplayStatics::SaveGameToSlot(ProfilesList, "PlayerProfilesList", 0);
	}

	if (PlayerProfile_Class->IsValidLowLevel() && ProfilesList->IsValidLowLevel() && PlayerProfilesScrollBox->IsValidLowLevel()) {
		for (int i = 0; i < ProfilesList->PlayerProfileNames.Num(); i++) {
			PlayerProfile_Reference = CreateWidget<UWidgetComponent_PlayerProfile>(this, PlayerProfile_Class);

			PlayerProfile_Reference->ProfileName = ProfilesList->PlayerProfileNames[i];
			PlayerProfile_Reference->ProfileNameText->SetText(FText::FromString(ProfilesList->PlayerProfileNames[i]));

			PlayerProfilesScrollBox->AddChild(PlayerProfile_Reference);
		}
	}
}