#include "Widget_PlayerProfileCreator.h"

#include "Components/TextBlock.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "SaveData_PlayerProfile.h"
#include "SaveData_PlayerProfilesList.h"
#include "Starmark_GameInstance.h"
#include "Starmark_Variables.h"
#include "Widget_AvatarLibrary.h"
#include "Widget_MainMenu.h"
#include "WidgetComponent_PlayerProfile.h"


// ------------------------- Widget
void UWidget_PlayerProfileCreator::OnWidgetOpened()
{
	ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

	// Get the list of Player Profiles
	if (!ProfilesList->IsValidLowLevel()) {
		ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::CreateSaveGameObject(USaveData_PlayerProfilesList::StaticClass()));
		UGameplayStatics::SaveGameToSlot(ProfilesList, "PlayerProfilesList", 0);
	}

	// Populate the scroll box with the players' profiles.
	if (ProfilesList->IsValidLowLevel() && PlayerProfilesScrollBox->IsValidLowLevel()) {
		PlayerProfilesScrollBox->ClearChildren();

		for (int i = 0; i < ProfilesList->PlayerProfileNames.Num(); i++) {
			PlayerProfileWidgetComponent_Reference = CreateWidget<UWidgetComponent_PlayerProfile>(this, PlayerProfileWidgetComponent_Class);

			PlayerProfileWidgetComponent_Reference->ProfileName = ProfilesList->PlayerProfileNames[i];
			PlayerProfileWidgetComponent_Reference->ProfileNameText->SetText(FText::FromString(ProfilesList->PlayerProfileNames[i]));

			// Bind delegate
			PlayerProfileWidgetComponent_Reference->OnPlayerProfileLoadedDelegate.AddDynamic(this, &UWidget_PlayerProfileCreator::OnPlayerProfileLoadedDelegateBroadcast);

			PlayerProfilesScrollBox->AddChild(PlayerProfileWidgetComponent_Reference);
		}
	}

	// Only enable the Avatar Library if the player has a profile that the library can be saved to
	AvatarLibraryButton->SetIsEnabled(Cast<UStarmark_GameInstance>(GetGameInstance())->PlayerSaveGameReference->IsValidLowLevel());
}


void UWidget_PlayerProfileCreator::OnSaveGameButtonPressed()
{
	USaveData_PlayerProfile* PlayerProfileData = Cast<USaveData_PlayerProfile>(UGameplayStatics::LoadGameFromSlot(NewProfileNameEntryField->GetText().ToString(), 0));
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (!PlayerProfileData) {
		PlayerProfileData = Cast<USaveData_PlayerProfile>(UGameplayStatics::CreateSaveGameObject(USaveData_PlayerProfile::StaticClass()));
	}

	// Profile variables
	// Profile
	PlayerProfileData->PlayerProfileStruct.ProfileName = NewProfileNameEntryField->GetText().ToString();
	// Player
	PlayerProfileData->PlayerProfileStruct.PlayerName = NewPlayerNameEntryField->GetText().ToString();

	UGameplayStatics::SaveGameToSlot(PlayerProfileData, NewProfileNameEntryField->GetText().ToString(), 0);

	// Add this profile to the ProfilesList if it isn't already there
	ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

	if (!ProfilesList->IsValidLowLevel())
		ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::CreateSaveGameObject(USaveData_PlayerProfilesList::StaticClass()));

	if (ProfilesList->IsValidLowLevel()) {
		ProfilesList->PlayerProfileNames.AddUnique(NewProfileNameEntryField->GetText().ToString());
		UGameplayStatics::SaveGameToSlot(ProfilesList, "PlayerProfilesList", 0);
	}

	// Set the newly created profile to be the player's active profile
	GameInstanceReference->PlayerSaveGameReference = PlayerProfileData;

	// To-Do: Save the player's profile data to a json file
	GameInstanceReference->ReturnPlayerSaveGameReference()->PlayerProfileStruct.PlayerName = PlayerProfileData->PlayerProfileStruct.PlayerName;
	GameInstanceReference->ReturnPlayerSaveGameReference()->PlayerProfileStruct.ProfileName = PlayerProfileData->PlayerProfileStruct.ProfileName;
	GameInstanceReference->ReturnPlayerSaveGameReference()->SaveProfileDataToJson();

	GameInstanceReference->PlayerDataStruct.PlayerName = PlayerProfileData->PlayerProfileStruct.PlayerName;
	GameInstanceReference->PlayerDataStruct.ProfileName = PlayerProfileData->PlayerProfileStruct.ProfileName;
	
	OnWidgetOpened();
}


void UWidget_PlayerProfileCreator::OnExitButtonPressed()
{
	if (!MainMenuWidget_Reference && MainMenuWidget_Class)
		MainMenuWidget_Reference = CreateWidget<UWidget_MainMenu>(this, MainMenuWidget_Class);

	if (MainMenuWidget_Reference) {
		MainMenuWidget_Reference->AddToViewport();
		this->RemoveFromParent();
	}
}


void UWidget_PlayerProfileCreator::PopulateTeamCreatorDropdowns()
{
	if (AvatarDataTable) {
		TArray<FAvatar_Struct> AvatarsArray;
		FString ContextString;
		TArray<FName> RowNames;
		RowNames = AvatarDataTable->GetRowNames();

		for (auto& Name : RowNames) {
			if (Name != "BigBoxBoy") {
				FAvatar_Struct* Avatar = AvatarDataTable->FindRow<FAvatar_Struct>(Name, ContextString);

				if (Avatar)
					AvatarsArray.Add(*Avatar);
			}
		}
	}
}


void UWidget_PlayerProfileCreator::OnAvatarLibraryButtonPressed()
{
	if (AvatarLibrary_Class->IsValidLowLevel()) {
		AvatarLibrary_Reference = CreateWidget<UWidget_AvatarLibrary>(this, AvatarLibrary_Class);
		AvatarLibrary_Reference->OnWidgetOpened();
		AvatarLibrary_Reference->AddToViewport();
	}
}


// ------------------------- Delegates
void UWidget_PlayerProfileCreator::OnPlayerProfileLoadedDelegateBroadcast()
{
	AvatarLibraryButton->SetIsEnabled(Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->PlayerSaveGameReference->IsValidLowLevel());
}