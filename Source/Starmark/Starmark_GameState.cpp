#include "Starmark_GameState.h"

#include "Actor_AbilitiesLibrary.h"
#include "Actor_GridTile.h"
#include "Actor_StatusEffectsLibrary.h"
#include "Character_NonAvatarEntity.h"
#include "Character_Pathfinder.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController_Base.h"
#include "Starmark_PlayerState.h"
#include "Starmark_GameMode.h"


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
		if (!Cast<ACharacter_NonAvatarEntity>(CurrentAvatar)) {
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

		if (!Cast<ACharacter_NonAvatarEntity>(CurrentAvatar)) {
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
		AvatarTurnOrder.Add(SlowedAvatarsInTurnOrder[i]);
	}

	DynamicAvatarTurnOrder = AvatarTurnOrder;
}


void AStarmark_GameState::AvatarBeginTurn_Implementation()
{
	if (AvatarTurnOrder.IsValidIndex(CurrentAvatarTurnIndex)) {
		ACharacter_Pathfinder* Avatar = AvatarTurnOrder[CurrentAvatarTurnIndex];
		
		Avatar->AvatarData.CurrentTileMoves = AvatarTurnOrder[CurrentAvatarTurnIndex]->AvatarData.MaximumTileMoves;

		// Reduce durations of all statuses
		for (int i = Avatar->CurrentStatusEffectsArray.Num() - 1; i >= 0; i--) {
			Avatar->CurrentStatusEffectsArray[i].TurnsRemaining--;

			if (Avatar->CurrentStatusEffectsArray[i].TurnsRemaining <= 0) {
				if (IsValid(Avatar->CurrentStatusEffectsArray[i].SpecialFunctionsActor))
					Avatar->CurrentStatusEffectsArray[i].SpecialFunctionsActor->OnStatusEffectRemoved(Avatar, Avatar->CurrentStatusEffectsArray[i]);
				else
					Avatar->CurrentStatusEffectsArray.RemoveAt(i);
			} else {
				// On Status Effect Start-of-turn effects
				if (Avatar->CurrentStatusEffectsArray[i].Name == "Paralyzed") {
					Avatar->AvatarData.CurrentTileMoves = Avatar->AvatarData.MaximumTileMoves / 2;
					break;
				} else if (IsValid(Avatar->CurrentStatusEffectsArray[i].SpecialFunctionsActor)) {
					Avatar->CurrentStatusEffectsArray[i].SpecialFunctionsActor->OnStatusEffectStartOfTurn(Avatar, Avatar->CurrentStatusEffectsArray[i]);
				}
			}
		}

		// Check for any abilities that trigger at the start of the turn
		if (Avatar->AvatarData.Ability.TriggerCondition == E_Ability_TriggerConditions::OnAvatarStartOfTurn) {
			// Ensure that there is only one AbilityActor because there only needs to be one that all Avatars can use
			if (Avatar->AvatarData.Ability.AbilityLibraryActor == nullptr) {
				TArray<AActor*> AbilityLibraryActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_AbilitiesLibrary::StaticClass(), AbilityLibraryActors);

				if (AbilityLibraryActors.Num() > 0) {
					for (int i = 0; i < AbilityLibraryActors.Num(); i++) {
						if (i == 0) 
							Avatar->AvatarData.Ability.AbilityLibraryActor = Cast<AActor_AbilitiesLibrary>(AbilityLibraryActors[i]);
						else
							AbilityLibraryActors[i]->Destroy();
					}
				} else {
					const FActorSpawnParameters SpawnInfo;
					Avatar->AvatarData.Ability.AbilityLibraryActor = GetWorld()->SpawnActor<AActor_AbilitiesLibrary>(AActor_AbilitiesLibrary::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);
				}
			}

			// Call the ability function
			Avatar->AvatarData.Ability.AbilityLibraryActor->SwitchOnAbilityEffect(Avatar->AvatarData.Ability.Function, Avatar, Avatar);
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

	// Check if an Avatar died this turn
	// If true, check for reserve Avatars before ending the turn

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
		Cast<AActor_GridTile>(GridTilesArray[j])->UpdateTileColour(E_GridTile_ColourChangeContext::Normal);
	}


	// Update the dynamic turn order
	if (DynamicAvatarTurnOrder.Num() > 0) {
		ACharacter_Pathfinder* FirstAvatarInDynamicTurnOrder = DynamicAvatarTurnOrder[0];
		DynamicAvatarTurnOrder.RemoveAt(0);
		DynamicAvatarTurnOrder.Insert(FirstAvatarInDynamicTurnOrder, DynamicAvatarTurnOrder.Num());
	}

	// Assign currently controlled avatars based on the dynamic turn order
	for (int i = DynamicAvatarTurnOrder.Num() - 1; i >= 0; i--) {
		if (IsValid(DynamicAvatarTurnOrder[i])) {
			DynamicAvatarTurnOrder[i]->PlayerControllerReference->CurrentSelectedAvatar = DynamicAvatarTurnOrder[i];

			DynamicAvatarTurnOrder[i]->CurrentSelectedAttack.Name = "None";
			DynamicAvatarTurnOrder[i]->CurrentSelectedAttack.AttachAttackTraceActorToMouse = false;
			DynamicAvatarTurnOrder[i]->ValidAttackTargetsArray.Empty();
		}
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