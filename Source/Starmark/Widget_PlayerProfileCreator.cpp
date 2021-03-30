#include "Widget_PlayerProfileCreator.h"

#include "Widget_MainMenu.h"
#include "WidgetComponent_PlayerProfile.h"
#include "Player_SaveData.h"
#include "SaveData_PlayerProfilesList.h"
#include "Components/TextBlock.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"


// ------------------------- Widget
void UWidget_PlayerProfileCreator::OnWidgetOpened()
{
	//if (PlayerProfile->IsValidLowLevel()) {
	//	UPlayer_SaveData* PlayerProfileData = Cast<UPlayer_SaveData>(PlayerProfile);

	//	NameEntryField->SetText(FText::FromString(PlayerProfileData->Name));
	//}
	//else {
	//NameEntryField->SetText(FText::FromString("Default"));
	//}

	// Get the list of Player Profiles
	ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

	if (!ProfilesList->IsValidLowLevel()) {
		ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::CreateSaveGameObject(USaveData_PlayerProfilesList::StaticClass()));
		UGameplayStatics::SaveGameToSlot(ProfilesList, "PlayerProfilesList", 0);
	}

	// Populate the scroll box with the players' profiles.
	if (ProfilesList->IsValidLowLevel() && PlayerProfilesScrollBox->IsValidLowLevel()) {
		for (int i = 0; i < ProfilesList->PlayerProfileNames.Num(); i++) {
			PlayerProfileWidgetComponent_Reference = CreateWidget<UWidgetComponent_PlayerProfile>(this, PlayerProfileWidgetComponent_Class);

			PlayerProfileWidgetComponent_Reference->ProfileName = ProfilesList->PlayerProfileNames[i];
			PlayerProfileWidgetComponent_Reference->ProfileNameText->SetText(FText::FromString(ProfilesList->PlayerProfileNames[i]));

			PlayerProfilesScrollBox->AddChild(PlayerProfileWidgetComponent_Reference);
		}
	}
}


void UWidget_PlayerProfileCreator::OnSaveGameButtonPressed()
{
	UPlayer_SaveData* PlayerProfileData = Cast<UPlayer_SaveData>(UGameplayStatics::LoadGameFromSlot(NameEntryField->GetText().ToString(), 0));

	if (!PlayerProfileData)
		PlayerProfileData = Cast<UPlayer_SaveData>(UGameplayStatics::CreateSaveGameObject(UPlayer_SaveData::StaticClass()));

	PlayerProfileData->Name = NameEntryField->GetText().ToString();
	UGameplayStatics::SaveGameToSlot(PlayerProfileData, NameEntryField->GetText().ToString(), 0);

	// Add this profile to the ProfilesList if it isn't already there
	ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

	if (!ProfilesList->IsValidLowLevel())
		ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::CreateSaveGameObject(USaveData_PlayerProfilesList::StaticClass()));

	if (ProfilesList->IsValidLowLevel()) {
		ProfilesList->PlayerProfileNames.AddUnique(NameEntryField->GetText().ToString());
		UGameplayStatics::SaveGameToSlot(ProfilesList, "PlayerProfilesList", 0);
	}

	OnWidgetOpened();
}


void UWidget_PlayerProfileCreator::OnExitButtonPressed()
{
	if (!MainMenuWidget_Reference && MainMenuWidget_Class) {
		MainMenuWidget_Reference = CreateWidget<UWidget_MainMenu>(this, MainMenuWidget_Class);
	}

	if (MainMenuWidget_Reference) {
		MainMenuWidget_Reference->AddToViewport();
		this->RemoveFromParent();
	}
}