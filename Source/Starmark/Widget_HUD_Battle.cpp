#include "Widget_HUD_Battle.h"


#include "Kismet/GameplayStatics.h"
#include "PlayerController_Base.h"
#include "WidgetComponent_AvatarAttack.h"
#include "Starmark_GameState.h"


// ------------------------- Widget
void UWidget_HUD_Battle::UpdateAvatarAttacksComponents()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	for (int i = 0; i < AvatarAttacksBox->GetChildrenCount(); i++) {
		if (PlayerControllerReference->CurrentSelectedAvatar->AllKnownAttacks.IsValidIndex(i)) {
			Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i))->AttackNameText->SetText(FText::FromString(PlayerControllerReference->CurrentSelectedAvatar->AllKnownAttacks[i].Name));
			AvatarAttacksBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Visible);
		}
		else {
			AvatarAttacksBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}


// ------------------------- Commands
void UWidget_HUD_Battle::MoveCommand()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_MoveCharacter;

	// Hide Avatar Attacks Box
	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Collapsed);
}


void UWidget_HUD_Battle::AttackCommand()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// Show Avatar Attacks Box
	if (AvatarAttacksBox) {
		UpdateAvatarAttacksComponents();
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Visible);
	}

	//PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack;

	//// Display Attack Range
	//PlayerControllerReference->CurrentSelectedAvatar->ShowAttackRange();
}


void UWidget_HUD_Battle::SwitchCommand()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_SelectCharacterToControl;

	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UWidget_HUD_Battle::EndCommand()
{
	GetWorld()->GetGameState<AStarmark_GameState>()->AvatarEndTurn();

	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Collapsed);
}