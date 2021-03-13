#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "Engine/Datatable.h"

#include "Starmark_GameState.generated.h"

// Forward Declarations
class ACharacter_Pathfinder;
class UWidget_HUD_Battle;
class APlayerController_Base;


UCLASS()
class STARMARK_API AStarmark_GameState : public AGameState
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- References
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	APlayerController_Base* PlayerControllerReference;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UDataTable* TypeEffectivenessDataTableReference;

// ------------------------- Battle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	TArray<ACharacter_Pathfinder*> AvatarTurnOrder;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	//UWidget_HUD_Battle* BattleHUD_Reference;

	// The Index of the Avater whose turn it is, in the AvatarTurnOrder Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	int CurrentAvatarTurnIndex;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	//TArray<APlayerController_Base*> PlayerControllersArray;

// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetTurnOrder(const TArray<APlayerController_Base*>& PlayerControllers);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AvatarEndTurn(const TArray<APlayerController_Base*>& PlayerControllers);

	UFUNCTION(BlueprintCallable)
	void AvatarBeginTurn();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void GameState_LaunchAttack(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);
};
