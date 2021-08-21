#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "Starmark_Variables.h"

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
// Variables
// --------------------------------------------------

// ------------------------- Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAvatar_Struct> PlayerState_PlayerParty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPlayer_SaveData* PlayerProfileReference;

// ------------------------- Lobby
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerReadyStatus = "Not Ready";

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarDataTable;


// Functions
// --------------------------------------------------

// ------------------------- Player
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerData(UPlayer_SaveData* PlayerProfile);

	// ------------------------- Lobby
	UFUNCTION(BlueprintCallable)
	void ChangePlayerReadyStatus();

// ------------------------- Battle
	UFUNCTION(BlueprintCallable)
	void PlayerState_BeginBattle();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SubtractHealth(ACharacter_Pathfinder* Defender, int DamageDealt);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Battle_AvatarDefeated(ACharacter_Pathfinder* Avatar);
};
