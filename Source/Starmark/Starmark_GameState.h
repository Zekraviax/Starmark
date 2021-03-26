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

// ------------------------- Battle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Battle")
	TArray<ACharacter_Pathfinder*> AvatarTurnOrder;

	// The Index of the Avater whose turn it is, in the AvatarTurnOrder Array.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int CurrentAvatarTurnIndex;

// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetTurnOrder(const TArray<APlayerController_Base*>& PlayerControllers);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AvatarEndTurn();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EndOfBattle();
};
