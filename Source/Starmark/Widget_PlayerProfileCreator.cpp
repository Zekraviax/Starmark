#include "Widget_PlayerProfileCreator.h"

#include "Components/TextBlock.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Player_SaveData.h"
#include "SaveData_PlayerProfilesList.h"
#include "Starmark_GameInstance.h"
#include "Starmark_Variables.h"
#include "Widget_MainMenu.h"
#include "WidgetComponent_PlayerProfile.h"


// ------------------------- Widget
void UWidget_PlayerProfileCreator::OnWidgetOpened()
{
	// Get the list of Player Profiles
	ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

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

			PlayerProfilesScrollBox->AddChild(PlayerProfileWidgetComponent_Reference);
		}
	}
}


void UWidget_PlayerProfileCreator::OnSaveGameButtonPressed()
{
	UPlayer_SaveData* PlayerProfileData = Cast<UPlayer_SaveData>(UGameplayStatics::LoadGameFromSlot(NewProfileNameEntryField->GetText().ToString(), 0));

	if (!PlayerProfileData)
		PlayerProfileData = Cast<UPlayer_SaveData>(UGameplayStatics::CreateSaveGameObject(UPlayer_SaveData::StaticClass()));

	// Profile variables
	// Player
	PlayerProfileData->Name = NewProfileNameEntryField->GetText().ToString();

	// Avatars
	if (AvatarDataTable) {
		TArray<FAvatar_Struct> AvatarsArray;
		FString ContextString;
		TArray<FName> RowNames;
		RowNames = AvatarDataTable->GetRowNames();

		for (auto& Name : RowNames) {
			FAvatar_Struct* Avatar = AvatarDataTable->FindRow<FAvatar_Struct>(Name, ContextString);

			if (Avatar->AvatarName == TeamSlotOneComboBox->GetSelectedOption())
				PlayerProfileData->TeamSlotOne = *Avatar;
			
			if (Avatar->AvatarName == TeamSlotTwoComboBox->GetSelectedOption())
				PlayerProfileData->TeamSlotTwo = *Avatar;

			if (Avatar->AvatarName == TeamSlotThreeComboBox->GetSelectedOption())
				PlayerProfileData->TeamSlotThree = *Avatar;

			if (Avatar->AvatarName == TeamSlotFourComboBox->GetSelectedOption())
				PlayerProfileData->TeamSlotFour = *Avatar;
		}
	}

	UGameplayStatics::SaveGameToSlot(PlayerProfileData, NewProfileNameEntryField->GetText().ToString(), 0);

	// Add this profile to the ProfilesList if it isn't already there
	ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

	if (!ProfilesList->IsValidLowLevel())
		ProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::CreateSaveGameObject(USaveData_PlayerProfilesList::StaticClass()));

	if (ProfilesList->IsValidLowLevel()) {
		ProfilesList->PlayerProfileNames.AddUnique(NewProfileNameEntryField->GetText().ToString());
		UGameplayStatics::SaveGameToSlot(ProfilesList, "PlayerProfilesList", 0);
	}

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

		for (auto& Avatar : AvatarsArray) {
			TeamSlotOneComboBox->AddOption(Avatar.AvatarName);
			if (TeamSlotOneComboBox->GetOptionCount() <= 2)
				TeamSlotOneComboBox->SetSelectedOption(Avatar.AvatarName);

			TeamSlotTwoComboBox->AddOption(Avatar.AvatarName);
			if (TeamSlotTwoComboBox->GetOptionCount() <= 2)
				TeamSlotTwoComboBox->SetSelectedOption(Avatar.AvatarName);

			TeamSlotThreeComboBox->AddOption(Avatar.AvatarName);
			if (TeamSlotThreeComboBox->GetOptionCount() <= 2)
				TeamSlotThreeComboBox->SetSelectedOption(Avatar.AvatarName);

			TeamSlotFourComboBox->AddOption(Avatar.AvatarName);
			if (TeamSlotFourComboBox->GetOptionCount() <= 2)
				TeamSlotFourComboBox->SetSelectedOption(Avatar.AvatarName);
		}
	}
}