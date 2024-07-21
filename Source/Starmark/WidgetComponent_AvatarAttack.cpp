#include "WidgetComponent_AvatarAttack.h"


#include "Actor_GridTile.h"
#include "Character_Pathfinder.h"
#include "PlayerController_Battle.h"


// ------------------------- Widget
void UWidgetComponent_AvatarAttack::OnButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("UWidgetComponent_AvatarAttack / OnButtonPressed / Attack button pressed.."));
	
	if (PlayerControllerReference->PlayerClickMode == E_PlayerCharacter_ClickModes::SelectReserveAvatarToSummon &&
		!AttackNameText->GetText().EqualTo(FText::FromString("RESERVE"))) {
		// Here is where the player selects a reserve avatar to summon.
		// We need to pass the index of the chosen avatar here.
		PlayerControllerReference->BeginSelectingTileForReserveAvatar(AvatarAttackIndex);
	} else if (AvatarAttackIndex > -1 && PlayerControllerReference && PlayerControllerReference->CurrentSelectedAvatar->CurrentKnownAttacks.IsValidIndex(AvatarAttackIndex)) {
		PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack;
		PlayerControllerReference->TileHighlightMode = E_PlayerCharacter_HighlightModes::E_AttackPattern;

		// Set selected Attack
		PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack = PlayerControllerReference->CurrentSelectedAvatar->CurrentKnownAttacks[AvatarAttackIndex];

		// Enable rotation towards mouse cursor
		PlayerControllerReference->CurrentSelectedAvatar->RotateAvatarTowardsMouse = true;
	} else if (AttackNameText->GetText().EqualTo(FText::FromString("RESERVE"))) {
		// Give the player the list of reserve avatars that they can summon.
		PlayerControllerReference->SetPlayerClickMode(E_PlayerCharacter_ClickModes::SelectReserveAvatarToSummon);
	}
}