#include "Starmark_GameState.h"


#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "Widget_HUD_Battle.h"
#include "PlayerController_Base.h"


void AStarmark_GameState::SetTurnOrder(UWidget_HUD_Battle* Battle_HUD)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundActors);

	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// Use Nested Loops to compare Avatars' Speeds.
	for (int i = 0; i < FoundActors.Num(); i++) {
		//AvatarTurnOrder.Add(Cast<ACharacter_Pathfinder>(FoundActors[i]));
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

	// Set Player's Avatar to the first in the list
	if (AvatarTurnOrder.Num() > 0) {
		PlayerControllerReference->CurrentSelectedAvatar = Cast<ACharacter_Pathfinder>(AvatarTurnOrder[0]);
	}

	CurrentAvatarTurnIndex = 0;

	// Update the Player's HUD
	if (Battle_HUD) {
		BattleHUD_Reference = Battle_HUD;

		for (int i = 0; i < AvatarTurnOrder.Num(); i++) {
			BattleHUD_Reference->TurnOrderTextBlock->SetText(FText::FromString(BattleHUD_Reference->TurnOrderTextBlock->GetText().ToString() + "\n" + AvatarTurnOrder[i]->AvatarData.AvatarName));
		}
	}

	// Update All Avatar's Decals
	PlayerControllerReference->UpdateSelectedAvatar();
}


void AStarmark_GameState::AvatarEndTurn()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// Reset Decals
	PlayerControllerReference->UpdateSelectedAvatar();

	CurrentAvatarTurnIndex++;

	// Reset Round
	if (CurrentAvatarTurnIndex >= AvatarTurnOrder.Num()) {
		CurrentAvatarTurnIndex = 0;
	}

	PlayerControllerReference->CurrentSelectedAvatar = Cast<ACharacter_Pathfinder>(AvatarTurnOrder[CurrentAvatarTurnIndex]);
	AvatarBeginTurn();
}


void AStarmark_GameState::AvatarBeginTurn()
{
	ACharacter_Pathfinder* AvatarRef = PlayerControllerReference->CurrentSelectedAvatar;

	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// Reset Decals
	PlayerControllerReference->UpdateSelectedAvatar();

	// Reset Moves
	AvatarRef->CurrentTileMoves = AvatarRef->AvatarData.MaximumTileMoves;

	// Apply Status Effects
	for (int i = AvatarRef->CurrentStatusEffectsArray.Num() - 1; i == 0; i--) {
		AvatarRef->CurrentStatusEffectsArray[i].TurnsRemaining--;

		if (!AvatarRef->CurrentStatusEffectsArray[i].TurnsRemaining == 0) {
			// Switch On Status Effect Name
			if (AvatarRef->CurrentStatusEffectsArray[i].Name == "Paralyzed") {
				AvatarRef->CurrentTileMoves = FMath::FloorToInt(AvatarRef->CurrentTileMoves / 2);
			}
		} else {

		}
	}
}