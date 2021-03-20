#include "Starmark_PlayerState.h"


#include "Widget_HUD_Battle.h"
#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"


// ------------------------- Battle
void AStarmark_PlayerState::CreateDefaultPlayerParty()
{
	FAvatar_Struct* DefaultAvatar = AvatarDataTable->FindRow<FAvatar_Struct>(TEXT("BalanceBoy"), "");
	PlayerState_PlayerParty.Add(*DefaultAvatar);
}


void AStarmark_PlayerState::PlayerState_OnPrimaryClick_Implementation(AActor* ClickedActor, ACharacter_Pathfinder* CurrentAvatar)
{
	APlayerController_Base* PlayerControllerReference = Cast<APlayerController_Base>(GetPawn()->GetController());

	if (ClickedActor) {
		if (ClickedActor->GetClass()->GetName().Contains("Character")) {
			// Select Avatar To Control
			//if (CurrentSelectedAvatar != ClickedActor && PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToControl) {
			//	Cast<ACharacter_Pathfinder>(ClickedActor)->OnAvatarClicked();
			//}
			// Select Avatar to Begin Attack
			if (PlayerControllerReference->CurrentSelectedAvatar != ClickedActor) {
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Attack: %s"), *PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.Name));
				// If we're attacking, and we clicked on a valid target in-range, launch an attack
				PlayerControllerReference->CurrentSelectedAvatar->LaunchAttack_Implementation(Cast<ACharacter_Pathfinder>(ClickedActor));
			}
		}
		// If all else fails, assume we clicked on a plane that we can move our controlled Avatar to
		else if (ClickedActor->GetClass()->GetName().Contains("StaticMesh") || ClickedActor->GetClass()->GetName().Contains("GridTile")) {
			if (PlayerControllerReference->CurrentSelectedAvatar->IsValidLowLevel()) {
				PlayerControllerReference->SendMoveCommandToServer_Implementation(ClickedActor->GetActorLocation());
			}
		}
	}
}