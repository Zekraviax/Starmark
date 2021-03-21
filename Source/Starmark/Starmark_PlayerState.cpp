#include "Starmark_PlayerState.h"


#include "Widget_HUD_Battle.h"
#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "Starmark_GameState.h"


// ------------------------- Battle
void AStarmark_PlayerState::PlayerState_BeginBattle()
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


void AStarmark_PlayerState::Server_SubtractHealth_Implementation(ACharacter_Pathfinder* Defender, int DamageDealt)
{
	if (Defender) {
		Defender->AvatarData.CurrentHealthPoints -= DamageDealt;
		Defender->UpdatePlayerParty();

		if (Defender->AvatarData.CurrentHealthPoints <= 0) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Avatar Defeated  /  Index: %d"), Defender->IndexInPlayerParty));

			Battle_AvatarDefeated(Defender);
		}
	}
}


void AStarmark_PlayerState::Battle_AvatarDefeated_Implementation(ACharacter_Pathfinder* Avatar)
{
	if (Avatar->PlayerControllerReference->PlayerParty.IsValidIndex(Avatar->IndexInPlayerParty)) {
		Avatar->PlayerControllerReference->PlayerParty.RemoveAt(Avatar->IndexInPlayerParty);
	}

	if (Avatar->PlayerControllerReference->PlayerParty.Num() <= 0) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player has run out of Avatars")));
		Cast<AStarmark_GameState>(GetWorld()->GetGameState())->EndOfBattle_Implementation();
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player Avatars Remaining: %d"), Avatar->PlayerControllerReference->PlayerParty.Num()));
	}

	Avatar->Destroy();
}
