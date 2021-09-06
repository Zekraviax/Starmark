#include "Starmark_GameState.h"


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
	DOREPLIFETIME(AStarmark_GameState, CurrentTurnOrderText);
}


// ------------------------- Lobby
void AStarmark_GameState::UpdateAllPlayersInLobby_Implementation()
{
	TArray<FString> PlayerNames, PlayerReadyStatuses;

}


// ------------------------- Battle
void AStarmark_GameState::SetTurnOrder_Implementation(const TArray<APlayerController_Base*>& PlayerControllers)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundActors);

	// Use Nested Loops to compare Avatars' Speeds.
	for (int i = 0; i < FoundActors.Num(); i++) {
		ACharacter_Pathfinder* AvatarReference = Cast<ACharacter_Pathfinder>(FoundActors[i]);

		if (AvatarTurnOrder.Num() <= 0)
			AvatarTurnOrder.Add(AvatarReference);
		else {
			for (int j = 0; j < AvatarTurnOrder.Num(); j++) {
				if (Cast<ACharacter_Pathfinder>(FoundActors[i])->AvatarData.BaseStats.Speed >= AvatarTurnOrder[j]->AvatarData.BaseStats.Speed && 
					!AvatarTurnOrder.Contains(AvatarReference)) {
					AvatarTurnOrder.Insert(AvatarReference, j);
					break;
				}

				// If we reach the end of the array and the Avatar isn't faster than any of the other Avatars, just add it at the end
				if (j == AvatarTurnOrder.Num() - 1 && !AvatarTurnOrder.Contains(Cast<ACharacter_Pathfinder>(FoundActors[i])))
					AvatarTurnOrder.Add(AvatarReference);
			}
		}
	}

	//CurrentTurnOrderText = AssembledTurnOrderText;
	
	//// Set CurrentActingPlayer states
	//for (int j = 0; j < PlayerArray.Num(); j++) {
	//	APlayerController_Base* PlayerController = Cast<APlayerController_Base>(PlayerArray[j]->GetPawn()->GetController());

	//	PlayerController->SetBattleWidgetVariables();
	//	PlayerController->UpdateAvatarsDecalsAndWidgets(AvatarTurnOrder[0]);
	//}
}


void AStarmark_GameState::AvatarEndTurn_Implementation()
{
	CurrentAvatarTurnIndex++;

	TArray<ACharacter_Pathfinder*> AvatarArray;
	TArray<bool> IsPlayerActingArray;
	APlayerController_Base* CurrentlyActingPlayer = nullptr;
	ACharacter_Pathfinder* CurrentlyActingAvatar = nullptr;

	// Reset Round
	if (CurrentAvatarTurnIndex >= AvatarTurnOrder.Num()) {
		CurrentAvatarTurnIndex = 0;
	}

	for (int j = 0; j < PlayerArray.Num(); j++) {
		APlayerController_Base* PlayerController = Cast<APlayerController_Base>(PlayerArray[j]->GetPawn()->GetController());

		if (PlayerController) {
			if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerController) {
				PlayerController->IsCurrentlyActingPlayer = true;
				CurrentlyActingPlayer = PlayerController;
				CurrentlyActingAvatar = CurrentlyActingPlayer->CurrentSelectedAvatar;
			} else {
				PlayerController->IsCurrentlyActingPlayer = false;
			}
		}
	}

	for (int j = 0; j < PlayerArray.Num(); j++) {
		APlayerController_Base* PlayerController = Cast<APlayerController_Base>(PlayerArray[j]->GetPawn()->GetController());
		PlayerController->UpdateAvatarsDecalsAndWidgets(CurrentlyActingAvatar);
	}
}


void AStarmark_GameState::EndOfBattle_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("End Of Battle. Returning to Main Menu...")));

	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->EndOfBattle();
}