#include "Starmark_GameState.h"


#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "PlayerController_Base.h"
#include "Starmark_PlayerState.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "Character_Pathfinder.h"


void AStarmark_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStarmark_GameState, AvatarTurnOrder);
}


void AStarmark_GameState::SetTurnOrder_Implementation(const TArray<APlayerController_Base*>& PlayerControllers)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundActors);

	// Use Nested Loops to compare Avatars' Speeds.
	for (int i = 0; i < FoundActors.Num(); i++) {
		if (AvatarTurnOrder.Num() <= 0) {
			AvatarTurnOrder.Add(Cast<ACharacter_Pathfinder>(FoundActors[i]));
		} else {
			for (int j = 0; j < AvatarTurnOrder.Num(); j++) {
				if (Cast<ACharacter_Pathfinder>(FoundActors[i])->AvatarData.BaseStats.Speed >= AvatarTurnOrder[j]->AvatarData.BaseStats.Speed && 
					!AvatarTurnOrder.Contains(Cast<ACharacter_Pathfinder>(FoundActors[i]))) {
					AvatarTurnOrder.Insert(Cast<ACharacter_Pathfinder>(FoundActors[i]), j);
					break;
				}
				// If we reach the end of the array and the Avatar isn't faster than any of the other Avatars, just add it at the end
				if (j == AvatarTurnOrder.Num() - 1 && !AvatarTurnOrder.Contains(Cast<ACharacter_Pathfinder>(FoundActors[i]))) {
					AvatarTurnOrder.Add(Cast<ACharacter_Pathfinder>(FoundActors[i]));
				}
			}
		}
	}

	for (int j = 0; j < PlayerArray.Num(); j++) {
		APlayerController_Base* PlayerController = Cast<APlayerController_Base>(PlayerArray[j]->GetPawn()->GetController());

		if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerController) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Current Damage 2 = %d")));
			PlayerController->ChangeActingPlayerState_Implementation(true);
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 2 = %d")));
			PlayerController->ChangeActingPlayerState_Implementation(false);
		}
	}
}


void AStarmark_GameState::AvatarEndTurn_Implementation(const TArray<APlayerController_Base*>& PlayerControllers)
{
	CurrentAvatarTurnIndex++;

	// Reset Round
	if (CurrentAvatarTurnIndex >= AvatarTurnOrder.Num()) {
		CurrentAvatarTurnIndex = 0;
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Avatars: %d  /  Turn Index: %d"), AvatarTurnOrder.Num(), CurrentAvatarTurnIndex));

	for (int j = 0; j < PlayerControllers.Num(); j++) {
		if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerControllers[j]) {
			PlayerControllers[j]->IsCurrentlyActingPlayer = true;
		}
		else {
			PlayerControllers[j]->IsCurrentlyActingPlayer = false;
		}
	}
}


void AStarmark_GameState::AvatarBeginTurn()
{
	//Character_Pathfinder* AvatarRef = PlayerControllerReference->CurrentSelectedAvatar;

	////if (!PlayerControllerReference)
	////	PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	//// Reset Decals
	////PlayerControllerReference->UpdateSelectedAvatar();

	//// Reset Moves
	//AvatarRef->CurrentTileMoves = AvatarRef->AvatarData.MaximumTileMoves;

	//// Apply Status Effects
	//for (int i = AvatarRef->CurrentStatusEffectsArray.Num() - 1; i == 0; i--) {
	//	AvatarRef->CurrentStatusEffectsArray[i].TurnsRemaining--;

	//	if (!AvatarRef->CurrentStatusEffectsArray[i].TurnsRemaining == 0) {
	//		// Switch On Status Effect Name
	//		if (AvatarRef->CurrentStatusEffectsArray[i].Name == "Paralyzed") {
	//			//AvatarRef->CurrentTileMoves = FMath::FloorToInt(AvatarRef->CurrentTileMoves / 2);
	//		}
	//	} else {
	//		AvatarRef->CurrentStatusEffectsArray.RemoveAt(i);
	//	}
	//}
}


void AStarmark_GameState::GameState_LaunchAttack_Implementation(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	if (this->HasAuthority())
		Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->GameMode_LaunchAttack_Implementation(Attacker, Defender);
}


void AStarmark_GameState::EndOfBattle_Implementation()
{
	for (int i = 0; i < AvatarTurnOrder.Num(); i++) {
		Cast<APlayerController_Base>(AvatarTurnOrder[i]->PlayerControllerReference)->IsCurrentlyActingPlayer = false;
	}

	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->EndOfBattle();
}