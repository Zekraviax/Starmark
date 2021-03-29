#include "Widget_PlayerProfileCreator.h"

#include "Widget_MainMenu.h"
#include "Player_SaveData.h"
#include "GameFramework/SaveGame.h"


// ------------------------- Widget
void UWidget_PlayerProfileCreator::OnWidgetOpened(USaveGame* PlayerProfile)
{
	if (PlayerProfile->IsValidLowLevel()) {
		UPlayer_SaveData* PlayerProfileData = Cast<UPlayer_SaveData>(PlayerProfile);

		NameEntryField->SetText(FText::FromString(PlayerProfileData->Name));
	}
	else {
		NameEntryField->SetText(FText::FromString("Default"));
	}
}


void UWidget_PlayerProfileCreator::OnSaveGameButtonPressed(USaveGame* PlayerProfile)
{
	UPlayer_SaveData* PlayerProfileData;

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