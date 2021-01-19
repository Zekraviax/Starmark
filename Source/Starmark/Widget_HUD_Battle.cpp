#include "Widget_HUD_Battle.h"


#include "Kismet/GameplayStatics.h"
#include "PlayerController_Base.h"


// ------------------------- Commands
void UWidget_HUD_Battle::MoveCommand()
{
	if (!PlayerControllerReference) {
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}

	PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_SelectCharacterToControl;
}


void UWidget_HUD_Battle::AttackCommand()
{
	if (!PlayerControllerReference) {
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}

	PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack;

	// Display Attack Range
	PlayerControllerReference->CurrentSelectedAvatar->ShowAttackRange();
}