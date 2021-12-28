#include "Starmark_GameState.h"


#include "Actor_GridTile.h"
#include "Character_DestructibleObject.h"
#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "Widget_HUD_Battle.h"
#include "Widget_ServerHost.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "WidgetComponent_LobbyPlayerVals.h"
#include "PlayerController_Base.h"
#include "PlayerController_Lobby.h"
#include "Player_SaveData.h"
#include "Character_Pathfinder.h"
#include "Starmark_PlayerState.h"
#include "Starmark_GameMode.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"


void AStarmark_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStarmark_GameState, AvatarTurnOrder);
	DOREPLIFETIME(AStarmark_GameState, DynamicAvatarTurnOrder);
	DOREPLIFETIME(AStarmark_GameState, CurrentTurnOrderText);
}


// ------------------------- Battle
void AStarmark_GameState::SetTurnOrder_Implementation(const TArray<APlayerController_Base*>& PlayerControllers)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundActors);
	TArray<ACharacter_Pathfinder*> SlowedAvatars, SlowedAvatarsInTurnOrder;

	AvatarTurnOrder.Empty();

	// Use Nested Loops to compare Avatars' Speeds.
	for (int i = 0; i < FoundActors.Num(); i++) {
		ACharacter_Pathfinder* CurrentAvatar = Cast<ACharacter_Pathfinder>(FoundActors[i]);
		if (!Cast<ACharacter_DestructibleObject>(CurrentAvatar)) {

			for (int j = 0; j < CurrentAvatar->CurrentStatusEffectsArray.Num(); j++) {
				if (CurrentAvatar->CurrentStatusEffectsArray[j].Name == "Drowning") {
					SlowedAvatars.AddUnique(CurrentAvatar);
					break;
				}
			}

			if (!SlowedAvatars.Contains(CurrentAvatar)) {
				if (AvatarTurnOrder.Num() <= 0)
					AvatarTurnOrder.Add(CurrentAvatar);
				else {
					for (int j = 0; j < AvatarTurnOrder.Num(); j++) {
						ACharacter_Pathfinder* AvatarInTurnOrder = Cast<ACharacter_Pathfinder>(AvatarTurnOrder[j]);

						if (CurrentAvatar->AvatarData.BaseStats.Speed >= AvatarInTurnOrder->AvatarData.BaseStats.Speed &&
							!AvatarTurnOrder.Contains(CurrentAvatar)) {
							AvatarTurnOrder.Insert(CurrentAvatar, j);
							break;
						}

						// If we reach the end of the array and the Avatar isn't faster than any of the other Avatars, just add it at the end
						if (j == AvatarTurnOrder.Num() - 1 && !AvatarTurnOrder.Contains(CurrentAvatar))
							AvatarTurnOrder.Add(CurrentAvatar);
					}
				}
			}
		}
	}
	// Compare slowed Actors' speed
	for (int i = 0; i < SlowedAvatars.Num(); i++) {
		ACharacter_Pathfinder* CurrentAvatar = Cast<ACharacter_Pathfinder>(SlowedAvatars[i]);

		if (!Cast<ACharacter_DestructibleObject>(CurrentAvatar)) {
			if (SlowedAvatarsInTurnOrder.Num() <= 0) {
				SlowedAvatarsInTurnOrder.Add(CurrentAvatar);
			} else {
				for (int j = 0; j < SlowedAvatarsInTurnOrder.Num(); j++) {
					ACharacter_Pathfinder* AvatarInTurnOrder = Cast<ACharacter_Pathfinder>(SlowedAvatarsInTurnOrder[j]);

					if (CurrentAvatar->AvatarData.BaseStats.Speed >= AvatarInTurnOrder->AvatarData.BaseStats.Speed &&
						!SlowedAvatarsInTurnOrder.Contains(CurrentAvatar)) {
						SlowedAvatarsInTurnOrder.Insert(CurrentAvatar, j);
						break;
					}

					// If we reach the end of the array and the Avatar isn't faster than any of the other Avatars, just add it at the end
					if (j == AvatarTurnOrder.Num() - 1 && !SlowedAvatarsInTurnOrder.Contains(CurrentAvatar))
						SlowedAvatarsInTurnOrder.Add(CurrentAvatar);
				}
			}
		}
	}

	for (int i = 0; i < SlowedAvatarsInTurnOrder.Num(); i++) {
		//AvatarTurnOrder.Insert(SlowedAvatarsInTurnOrder[i], AvatarTurnOrder.Num());
		AvatarTurnOrder.Add(SlowedAvatarsInTurnOrder[i]);
	}

	DynamicAvatarTurnOrder = AvatarTurnOrder;
}


void AStarmark_GameState::AvatarBeginTurn_Implementation()
{
	if (AvatarTurnOrder.IsValidIndex(CurrentAvatarTurnIndex)) {
		AvatarTurnOrder[CurrentAvatarTurnIndex]->AvatarData.CurrentTileMoves = AvatarTurnOrder[CurrentAvatarTurnIndex]->AvatarData.MaximumTileMoves;

		// Reduce durations of all statuses
		for (int i = AvatarTurnOrder[CurrentAvatarTurnIndex]->CurrentStatusEffectsArray.Num() - 1; i >= 0; i--) {
			AvatarTurnOrder[CurrentAvatarTurnIndex]->CurrentStatusEffectsArray[i].TurnsRemaining--;

			if (AvatarTurnOrder[CurrentAvatarTurnIndex]->CurrentStatusEffectsArray[i].TurnsRemaining <= 0) {
				AvatarTurnOrder[CurrentAvatarTurnIndex]->CurrentStatusEffectsArray.RemoveAt(i);
			} else {
				if (AvatarTurnOrder[CurrentAvatarTurnIndex]->CurrentStatusEffectsArray[i].Name == "Paralyzed") {
					AvatarTurnOrder[CurrentAvatarTurnIndex]->AvatarData.CurrentTileMoves = AvatarTurnOrder[CurrentAvatarTurnIndex]->AvatarData.MaximumTileMoves / 2;
					break;
				}
			}
		}
	}

	for (int j = 0; j < PlayerArray.Num(); j++) {
		APlayerController_Base* PlayerController = Cast<APlayerController_Base>(PlayerArray[j]->GetPawn()->GetController());
		PlayerController->Player_OnAvatarTurnChanged();
	}
}


void AStarmark_GameState::AvatarEndTurn_Implementation()
{
	TArray<ACharacter_Pathfinder*> AvatarArray;
	TArray<bool> IsPlayerActingArray;
	TArray<AActor*> GridTilesArray;

	CurrentAvatarTurnIndex++;

	// Reset Round if all Avatars have acted
	if (CurrentAvatarTurnIndex >= AvatarTurnOrder.Num())
		CurrentAvatarTurnIndex = 0;

	UE_LOG(LogTemp, Warning, TEXT("AvatarEndTurn / CurrentAvatarTurnIndex is: %d"), CurrentAvatarTurnIndex);

	for (int j = 0; j < PlayerArray.Num(); j++) {
		APlayerController_Base* PlayerController = Cast<APlayerController_Base>(PlayerArray[j]->GetPawn()->GetController());

		if (PlayerController) {
			if (AvatarTurnOrder.IsValidIndex(CurrentAvatarTurnIndex)) {
				if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerController) {
					PlayerController->IsCurrentlyActingPlayer = true;
					PlayerController->CurrentSelectedAvatar = AvatarTurnOrder[CurrentAvatarTurnIndex];
				} else {
					PlayerController->IsCurrentlyActingPlayer = false;
				}
			}
		}
	}

	// Set all GridTiles to default colours and colourability
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	for (int j = 0; j < GridTilesArray.Num(); j++) {
		Cast<AActor_GridTile>(GridTilesArray[j])->ChangeColourOnMouseHover = true;
	}


	// Update the dynamic turn order
	if (DynamicAvatarTurnOrder.Num() > 0) {
		ACharacter_Pathfinder* FirstAvatarInDynamicTurnOrder = DynamicAvatarTurnOrder[0];
		DynamicAvatarTurnOrder.RemoveAt(0);
		DynamicAvatarTurnOrder.Insert(FirstAvatarInDynamicTurnOrder, DynamicAvatarTurnOrder.Num());
	}

	// Assign currently controlled avatars based on the dynamic turn order
	for (int i = DynamicAvatarTurnOrder.Num() - 1; i >= 0; i--) {
		if (IsValid(DynamicAvatarTurnOrder[i]))
			DynamicAvatarTurnOrder[i]->PlayerControllerReference->CurrentSelectedAvatar = DynamicAvatarTurnOrder[i];
	}

	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Server_UpdateAllAvatarDecals();

	AvatarBeginTurn();
}


void AStarmark_GameState::EndOfBattle_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("EndOfBattle / Return each player to the Main Menu"));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("End Of Battle. Returning to Main Menu...")));

	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->EndOfBattle();
}