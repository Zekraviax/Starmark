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


UCLASS()
class STARMARK_API AStarmark_GameState : public AGameState
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Local Helper Variables
	UPROPERTY()
	AStarmark_GameMode* GameModeReference;

// ------------------------- Global Helper Variables
	UPROPERTY()
	ACharacter_Pathfinder* CurrentlyActingAvatar;

	UPROPERTY()
	APlayerController_Battle* CurrentlyActingPlayer;

// ------------------------- Lobbies
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_LobbyPlayerVals> LobbyPlayerVals_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent_LobbyPlayerVals* LobbyPlayerVals_Reference;

	UPROPERTY()
	int MultiplayerBattleExpectedPlayers = 1;

// ------------------------- Battle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<ACharacter_Pathfinder*> AvatarTurnOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<ACharacter_Pathfinder*> DynamicAvatarTurnOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRepNotify_DynamicAvatarTurnOrderUpdated)
	TArray<UTexture2D*> DynamicAvatarTurnOrderImages;

	// The Index of the Avatar whose turn it is, in the AvatarTurnOrder Array.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentAvatarTurnIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString CurrentTurnOrderText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool AvatarDiedThisTurn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* StatusEffectsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString GameStateContextString;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FAvatar_StatusEffect StunStatus;

	UPROPERTY()
	FTimerHandle StunTimerHandle;

// ------------------------- Game
	UPROPERTY()
	int ExpectedPlayersForNextLevel = 1;

// Functions
// --------------------------------------------------

// ------------------------- Local Helper Functions

// ------------------------- Global Helper Functions
	UFUNCTION()
	ACharacter_Pathfinder* ReturnCurrentlyActingAvatar();

	UFUNCTION()
	APlayerController_Battle* ReturnCurrentlyActingPlayer();

	UFUNCTION()
	TArray<APlayerController_Battle*> ReturnAllBattlePlayerControllers();

	UFUNCTION()
	void ShowHideAllPlayerHuds();

// ------------------------- Battle
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetTurnOrder();

	UFUNCTION()
	void OnRepNotify_DynamicAvatarTurnOrderUpdated();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AvatarBeginTurn();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AvatarEndTurn();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EndOfBattle();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StunDelayedSkipTurn();
};
