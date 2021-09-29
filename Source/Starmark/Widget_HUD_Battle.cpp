#include "Widget_HUD_Battle.h"


#include "Kismet/GameplayStatics.h"
#include "PlayerController_Base.h"
#include "WidgetComponent_AvatarAttack.h"
#include "Starmark_GameState.h"


// ------------------------- Widget
void UWidget_HUD_Battle::UpdateAvatarAttacksComponents()
{
	if (AvatarAttacksBox) {
		for (int i = 0; i < AvatarAttacksBox->GetChildrenCount(); i++) {
			if (PlayerControllerReference->CurrentSelectedAvatar->CurrentKnownAttacks.IsValidIndex(i)) {
				Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i))->AttackNameText->SetText(FText::FromString(PlayerControllerReference->CurrentSelectedAvatar->CurrentKnownAttacks[i].Name));

				Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i))->PlayerControllerReference = PlayerControllerReference;
				Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i))->AvatarAttackIndex = i;

				AvatarAttacksBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Visible);
			}
			else
				AvatarAttacksBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


void UWidget_HUD_Battle::UpdateTurnOrderText(FString NewText)
{
	if (TurnOrderTextBlock->IsValidLowLevel())
		TurnOrderTextBlock->SetText(FText::FromString("Turn Order:\n" + NewText));

	UpdateAvatarAttacksComponents();
}


void UWidget_HUD_Battle::OnPlayerClick()
{
	// Hide Avatar Attacks Box if an AttackButton wasn't clicked
	//if (AvatarAttacksBox)
	//	AvatarAttacksBox->SetVisibility(ESlateVisibility::Hidden);
}


void UWidget_HUD_Battle::OnPlayerCurrentlyActingStateChanged()
{
	// Set buttons to be unclickable if it isn't the player's turn
	if (PlayerControllerReference->IsValidLowLevel()) {
		if (PlayerControllerReference->IsCurrentlyActingPlayer) {
			MoveCommandButton->SetIsEnabled(true);
			AttackCommandButton->SetIsEnabled(true);
			EndTurnCommandButton->SetIsEnabled(true);
		} else {
			MoveCommandButton->SetIsEnabled(false);
			AttackCommandButton->SetIsEnabled(false);
			EndTurnCommandButton->SetIsEnabled(false);
		}
	}
}


// ------------------------- Commands
void UWidget_HUD_Battle::MoveCommand()
{
	PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_MoveCharacter;

	// Hide Avatar Attacks Box
	//if (AvatarAttacksBox)
	//	AvatarAttacksBox->SetVisibility(ESlateVisibility::Hidden);
}


void UWidget_HUD_Battle::AttackCommand()
{
	// Show Avatar Attacks Box
	if (AvatarAttacksBox) {
		UpdateAvatarAttacksComponents();

		//AvatarAttacksBox->SetVisibility(ESlateVisibility::Visible);
	}
}


void UWidget_HUD_Battle::SwitchCommand()
{
	PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_SelectCharacterToControl;

	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Hidden);
}


void UWidget_HUD_Battle::EndCommand()
{
	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Hidden);

	PlayerControllerReference->SendEndOfTurnCommandToServer_Implementation();
}