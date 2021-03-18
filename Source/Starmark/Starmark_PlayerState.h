#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "Starmark_GameMode.h"

#include "Starmark_PlayerState.generated.h"

// Forward Declarations
class UWidget_HUD_Battle;
class ACharacter_Pathfinder;


UCLASS()
class STARMARK_API AStarmark_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Player
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UWidget_HUD_Battle* PlayerState_BattleHUD;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//ACharacter_Pathfinder* PlayerState_CurrentControlledAvatar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAvatar_Struct> PlayerState_PlayerParty;

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarDataTable;


// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable)
	void CreateDefaultPlayerParty();

	UFUNCTION(BlueprintCallable)
	void GetPlayerControllerVariables();
};
