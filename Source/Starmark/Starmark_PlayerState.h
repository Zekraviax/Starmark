#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "Starmark_Variables.h"
#include "UnrealNetwork.h"

#include "Starmark_PlayerState.generated.h"


// Forward Declarations
class UWidget_HUD_Battle;
class ACharacter_Pathfinder;
class UPlayer_SaveData;


UCLASS()
class STARMARK_API AStarmark_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AStarmark_PlayerState();

// Variables
// --------------------------------------------------

// ------------------------- Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<FAvatar_Struct> PlayerState_PlayerParty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPlayer_SaveData* PlayerProfileReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString ReplicatedPlayerName = "Default";

// ------------------------- Lobby
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString PlayerReadyStatus = "Not Ready";

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarDataTable;


// Functions
// --------------------------------------------------

// ------------------------- Player
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerData();

// ------------------------- Lobby
	UFUNCTION(BlueprintCallable)
	void ChangePlayerReadyStatus();

	UFUNCTION(BlueprintNativeEvent)
	void SendUpdateToMultiplayerLobby();

// ------------------------- Battle
	UFUNCTION(Client, Reliable)
	void Client_UpdateReplicatedPlayerName();

	UFUNCTION(Server, Reliable)
	void Server_UpdateReplicatedPlayerName(const FString& UpdatedReplicatedPlayerName);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void PlayerState_BeginBattle();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SubtractHealth(ACharacter_Pathfinder* Defender, int DamageDealt);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Battle_AvatarDefeated(ACharacter_Pathfinder* Avatar);

	UFUNCTION(Server, Reliable)
	void Server_UpdatePlayerStateVariables(const TArray<FAvatar_Struct>& UpdatetPlayerParty);
};
