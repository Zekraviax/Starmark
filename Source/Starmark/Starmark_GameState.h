#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "Engine/DataTable.h"
#include "Starmark_Variables.h"

#include "Starmark_GameState.generated.h"


// Forward Declarations
class ACharacter_Pathfinder;
class AStarmark_GameMode;
class APlayerController_Battle;
class UWidgetComponent_LobbyPlayerVals;
class UWidget_HUD_Battle;


// The GameState is for variables that should be replicated to all players in multiplayer scenarios.
// All players plus the server have access to the GameState.
UCLASS()
class STARMARK_API AStarmark_GameState : public AGameState
{
	GENERATED_BODY()

public:
// ---------------------------------------- Variables ---------------------------------------- 
// ------------------------- Local Helper Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AStarmark_GameMode* GameModeReference;

// ------------------------- Global Helper Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ACharacter_Pathfinder* CurrentlyActingAvatar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerController_Battle* CurrentlyActingPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<int, FPlayer_Data> PlayerDataAndUniqueIDMap;

// ------------------------- Lobbies
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_LobbyPlayerVals> LobbyPlayerVals_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent_LobbyPlayerVals* LobbyPlayerVals_Reference;

	// To-Do: Move this to the Lobby GameMode?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int MultiplayerBattleExpectedPlayers = 1;

// ------------------------- Battle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<FPlayer_Data> PlayerDataStructsArray;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<ACharacter_Pathfinder*> AvatarTurnOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<ACharacter_Pathfinder*> DynamicAvatarTurnOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRepNotify_DynamicAvatarTurnOrderUpdated)
	TArray<UTexture2D*> DynamicAvatarTurnOrderImages;

	// The Index of the currently acting avatar in the AvatarTurnOrder array.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentAvatarTurnIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString CurrentTurnOrderText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* StatusEffectsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString GameStateContextString;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FAvatar_StatusEffect StunStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTimerHandle StunTimerHandle;

// ------------------------- Game
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ExpectedPlayersForNextLevel = 1;


// ---------------------------------------- Functions ---------------------------------------- 
// ------------------------- Local Helper Functions

// ------------------------- Global Helper Functions
	ACharacter_Pathfinder* ReturnCurrentlyActingAvatar();
	APlayerController_Battle* ReturnCurrentlyActingPlayer();

	// To-Do: Move this to the GameMode.
	void ShowHideAllPlayerHuds();
	FPlayer_Data FindPlayerDataUsingMultiplayerUniqueID(int MultiplayerUniqueID);

// ------------------------- Battle
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetTurnOrder();

	UFUNCTION(Server, Reliable) // This function needs the UFUNCTION tag. It breaks without it.
	void OnRepNotify_DynamicAvatarTurnOrderUpdated();

	// This function should only be used to update variables that all players might want.
	// For other functionality, use the GameMode.
	// To-Do: Update the following two functions to reflect the previous comments.
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AvatarBeginTurn();

	// This function should only be used to update variables that all players might want.
	// For other functionality, use the GameMode.
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AvatarEndTurn();

	// To-Do: Move this function to the GameMode
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EndOfBattle();

	// To-Do: Move this function to the GameMode
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StunDelayedSkipTurn();
};
