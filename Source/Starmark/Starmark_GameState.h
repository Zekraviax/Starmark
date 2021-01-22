#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Starmark_GameState.generated.h"

// Forward Declarations
class ACharacter_Pathfinder;
class UWidget_HUD_Battle;
class APlayerController_Base;


UCLASS()
class STARMARK_API AStarmark_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- References
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	APlayerController_Base* PlayerControllerReference;

// ------------------------- Battle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	TArray<ACharacter_Pathfinder*> AvatarTurnOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	UWidget_HUD_Battle* BattleHUD_Reference;

	// The Index of the Avater whose turn it is, in the AvatarTurnOrder Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	int CurrentAvatarTurnIndex;

// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable)
	void SetTurnOrder(UWidget_HUD_Battle* Battle_HUD);

	UFUNCTION(BlueprintCallable)
	void AvatarEndTurn();

};
