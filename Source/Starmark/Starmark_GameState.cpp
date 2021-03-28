#include "Starmark_GameState.h"


#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "PlayerController_Base.h"
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
	DOREPLIFETIME(AStarmark_GameState, CurrentTurnOrderText);
}


void AStarmark_GameState::SetTurnOrder_Implementation(const TArray<APlayerController_Base*>& PlayerControllers)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundActors);

	// Assemble turn order text
	FString AssembledTurnOrderText;

	// Use Nested Loops to compare Avatars' Speeds.
	for (int i = 0; i < FoundActors.Num(); i++) {
		if (AvatarTurnOrder.Num() <= 0)
			AvatarTurnOrder.Add(Cast<ACharacter_Pathfinder>(FoundActors[i]));
		else {
			for (int j = 0; j < AvatarTurnOrder.Num(); j++) {
				if (Cast<ACharacter_Pathfinder>(FoundActors[i])->AvatarData.BaseStats.Speed >= AvatarTurnOrder[j]->AvatarData.BaseStats.Speed && 
					!AvatarTurnOrder.Contains(Cast<ACharacter_Pathfinder>(FoundActors[i]))) {
					AvatarTurnOrder.Insert(Cast<ACharacter_Pathfinder>(FoundActors[i]), j);
					break;
				}
				// If we reach the end of the array and the Avatar isn't faster than any of the other Avatars, just add it at the end
				if (j == AvatarTurnOrder.Num() - 1 && !AvatarTurnOrder.Contains(Cast<ACharacter_Pathfinder>(FoundActors[i])))
					AvatarTurnOrder.Add(Cast<ACharacter_Pathfinder>(FoundActors[i]));
			}
		}

		if (AvatarTurnOrder[i]->PlayerControllerReference->PlayerProfileReference->IsValidLowLevel())
			AssembledTurnOrderText.Append(AvatarTurnOrder[i]->AvatarData.AvatarName + " / " + AvatarTurnOrder[i]->PlayerControllerReference->PlayerProfileReference->Name + "\n");
		else
			AssembledTurnOrderText.Append(AvatarTurnOrder[i]->AvatarData.AvatarName + "\n");
	}

	CurrentTurnOrderText = AssembledTurnOrderText;
	
	// Set CurrentActingPlayer states
	for (int j = 0; j < PlayerArray.Num(); j++) {
		APlayerController_Base* PlayerController = Cast<APlayerController_Base>(PlayerArray[j]->GetPawn()->GetController());

		if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerController) {
			PlayerController->ReceiveChangeActingPlayerStateFromServer_Implementation(true);
		} else
			PlayerController->ReceiveChangeActingPlayerStateFromServer_Implementation(false);
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Turn Order Text: %s"), *CurrentTurnOrderText));
}


void AStarmark_GameState::AvatarEndTurn_Implementation()
{
	CurrentAvatarTurnIndex++;

	// Reset Round
	if (CurrentAvatarTurnIndex >= AvatarTurnOrder.Num()) {
		CurrentAvatarTurnIndex = 0;
	}

	for (int j = 0; j < PlayerArray.Num(); j++) {
		APlayerController_Base* PlayerController = Cast<APlayerController_Base>(PlayerArray[j]->GetPawn()->GetController());

		if (PlayerController) {
			if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerController)
				PlayerController->ReceiveChangeActingPlayerStateFromServer_Implementation(true);
			else
				PlayerController->ReceiveChangeActingPlayerStateFromServer_Implementation(false);
		}
	}
}


void AStarmark_GameState::EndOfBattle_Implementation()
{
	for (int i = 0; i < AvatarTurnOrder.Num(); i++) {
		Cast<APlayerController_Base>(AvatarTurnOrder[i]->PlayerControllerReference)->ChangeActingPlayerState(false);
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("End Of Battle. Returning to Main Menu...")));

	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->EndOfBattle();
}