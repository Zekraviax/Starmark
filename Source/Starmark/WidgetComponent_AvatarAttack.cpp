#include "WidgetComponent_AvatarAttack.h"

#include "PlayerController_Base.h"


// ------------------------- Widget
void UWidgetComponent_AvatarAttack::OnButtonPressed()
{
	if (AvatarAttackIndex > -1 && PlayerControllerReference) {
		PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack;

		// Set selected Attack
		PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack = PlayerControllerReference->CurrentSelectedAvatar->CurrentKnownAttacks[AvatarAttackIndex];

		// Clear valid targets for old attacks
		PlayerControllerReference->CurrentSelectedAvatar->ValidAttackTargetsArray.Empty();

		// Get new valid targets
		PlayerControllerReference->CurrentSelectedAvatar->ShowAttackRange();

		// Enable rotation towards mouse cursor
		PlayerControllerReference->CurrentSelectedAvatar->RotateAvatarTowardsMouse = true;
		PlayerControllerReference->TileHighlightMode = E_PlayerCharacter_HighlightModes::E_AttackPattern;
	}
}