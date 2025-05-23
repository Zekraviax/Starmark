#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "Starmark_Variables.h"

#include "Starmark_PlayerState.generated.h"


// Forward Declarations
class ACharacter_Pathfinder;
class UPlayer_SaveData;
class USaveData_PlayerProfile;
class UStarmark_GameInstance;
class UWidget_HUD_Battle;


// The PlayerState is replicated to everyone in multiplayer scenarios.
// This means it is a good place to put variables that should be replicated like
// a player's team of avatars, not their Save Data.
// The GameState holds an array of PlayerStates, called the "PlayerArray".
UCLASS()
class STARMARK_API AStarmark_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

	AStarmark_PlayerState();


// ---------------------------------------- Variables ---------------------------------------- 
// ------------------------- Local Helper Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStarmark_GameInstance* GameInstanceReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString PlayerStateContextString;

// ------------------------- Global Helper Variables
	// To-Do: Make the PlayerState easily accessible wherever data needs to be saved
	// And make a function in the PlayerState to handle the PlayerProfileInstance not existing

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> AvatarsDataTableRowNames;

// ------------------------- Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<FAvatar_Struct> PlayerTeam;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FPlayer_Data PlayerData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString ReplicatedPlayerName = "Default";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	int ReplicatedMultiplayerUniqueID = -1;

// ------------------------- Lobby
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString PlayerReadyStatus = "Not Ready";

// ------------------------- Battle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PreBattleCheck = false;

	// This variable only tracks the number of avatars that the player controlled that died this turn, not all avatars.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	int NumberOfAvatarsDiedThisTurn = 0;


// ---------------------------------------- Functions ---------------------------------------- 
// ------------------------- Local Helper Functions

// ------------------------- Global Helper Functions
	FPlayer_Data GetPlayerDataFromGameInstance();

// ------------------------- Player
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerData();

// ------------------------- Lobby
	UFUNCTION(BlueprintNativeEvent)
	void SendUpdateToMultiplayerLobby();

// ------------------------- Battle
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SubtractHealth(ACharacter_Pathfinder* Defender, int DamageDealt);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_AddHealth(ACharacter_Pathfinder* Avatar, int Healing);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Battle_AvatarDefeated(ACharacter_Pathfinder* Avatar);

	UFUNCTION(Server, Reliable)
	void Server_UpdatePlayerStateVariables(const TArray<FAvatar_Struct>& UpdatedPlayerParty);
};
