#include "WidgetComponent_AvatarAttack.h"


#include "Actor_AttackEffectsLibrary.h"
#include "Actor_GridTile.h"
#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController_Base.h"
#include "Starmark_GameInstance.h"


// ------------------------- Widget
void UWidgetComponent_AvatarAttack::OnButtonPressed()
{
	if (AvatarAttackIndex > -1 && PlayerControllerReference && PlayerControllerReference->CurrentSelectedAvatar->CurrentKnownAttacks.IsValidIndex(AvatarAttackIndex)) {
		PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack;
		PlayerControllerReference->TileHighlightMode = E_PlayerCharacter_HighlightModes::E_AttackPattern;

		// Set selected Attack
		PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack = PlayerControllerReference->CurrentSelectedAvatar->CurrentKnownAttacks[AvatarAttackIndex];

		// Clear valid targets for old attacks
		//PlayerControllerReference->CurrentSelectedAvatar->ValidAttackTargetsArray.Empty();

		// Get new valid targets
		PlayerControllerReference->CurrentSelectedAvatar->ShowAttackRange();

		// Enable rotation towards mouse cursor
		PlayerControllerReference->CurrentSelectedAvatar->RotateAvatarTowardsMouse = true;


		// Hat Trick: Get the tile the player is on and add it to the hat trick array
		if (PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.AttackEffectsOnTarget.Contains(EBattle_AttackEffects::SpawnHats)) {
			ACharacter_Pathfinder* AvatarReference = PlayerControllerReference->CurrentSelectedAvatar;
			AActor_GridTile* CurrentTile = Cast<UStarmark_GameInstance>(GetWorld()->GetGameInstance())->FindTileByCoordinates(AvatarReference->GetActorLocation().X / 200, AvatarReference->GetActorLocation().Y / 200);
			Cast<APlayerController_Base>(GetWorld()->GetFirstPlayerController())->OnPrimaryClick(CurrentTile);
		}
	}
}