#include "Starmark_GameState.h"


#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "PlayerController_Base.h"
#include "Starmark_PlayerState.h"
#include "GameFramework/Controller.h"


void AStarmark_GameState::SetTurnOrder(TArray<APlayerController_Base*> PlayerControllers)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundActors);

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

	CurrentAvatarTurnIndex = 0;

	// Set Player's Avatar to the first in the list
	//for (int i = 0; i < PlayerControllers.Num(); i++) {
	//	// Update All Avatar's Decals
	//	PlayerControllers[i]->UpdateSelectedAvatar();

	//	// Update the Player's HUD
	//	//if (PlayerControllers[i]->BattleHUDReference) {
	//	//	//BattleHUD_Reference = PlayerControllers[i]->BattleHUDReference;

	//	//	PlayerControllers[i]->BattleHUDReference->AvatarBattleDataWidget->LinkedAvatar = PlayerControllers[i]->CurrentSelectedAvatar;
	//	//	PlayerControllers[i]->BattleHUDReference->AvatarBattleDataWidget->SetAvatarData();
	//	//	//PlayerControllers[i]->BattleHUDReference->TurnOrderTextBlock->SetText(FText::FromString(FString::FromInt(AvatarTurnOrder.Num())));
	//	//}
	//	//else {
	//	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Battle HUD Reference Not Valid")));
	//	//}
	//}

	//for (int i = 0; i < AvatarTurnOrder.Num(); i++) {
	//for (int j = 0; j < PlayerControllers.Num(); j++) {
	//	AStarmark_PlayerState* PlayerState = Cast<AStarmark_PlayerState>(PlayerControllers[j]->PlayerState);

	//	if (PlayerState->PlayerState_BattleHUD->IsValidLowLevel())
	//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Battle HUD Valid")));
	//	else 
	//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Battle HUD Not Valid")));
	//	//PlayerControllers[j]->BattleHUDReference->TurnOrderTextBlock->SetText(FText::FromString(FString::FromInt(i)));
	//	//PlayerControllers[j]->BattleHUDReference->TurnOrderTextBlock->SetText(FText::FromString(PlayerControllers[j]->BattleHUDReference->TurnOrderTextBlock->GetText().ToString() + "\n" + AvatarTurnOrder[i]->AvatarData.AvatarName));
	//}
	//}
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

	//PlayerControllerReference->CurrentSelectedAvatar = Cast<ACharacter_Pathfinder>(AvatarTurnOrder[CurrentAvatarTurnIndex]);
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
			AvatarRef->CurrentStatusEffectsArray.RemoveAt(i);
		}
	}

	// Re-Calculate Stats

	// Update HUD
	if (BattleHUD_Reference) {
		//BattleHUD_Reference->AvatarBattleDataWidget->LinkedAvatar = AvatarRef->AvatarData;
		//BattleHUD_Reference->AvatarBattleDataWidget->SetAvatarData();
	}
}