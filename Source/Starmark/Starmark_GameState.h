#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "Engine/Datatable.h"

#include "Starmark_GameState.generated.h"


// Forward Declarations
class ACharacter_Pathfinder;
class UWidget_HUD_Battle;
class APlayerController_Base;
class UWidgetComponent_LobbyPlayerVals;


UCLASS()
class STARMARK_API AStarmark_GameState : public AGameState
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_LobbyPlayerVals> LobbyPlayerVals_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent_LobbyPlayerVals* LobbyPlayerVals_Reference;

// ------------------------- Battle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<ACharacter_Pathfinder*> AvatarTurnOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<ACharacter_Pathfinder*> DynamicAvatarTurnOrder;

	// The Index of the Avatar whose turn it is, in the AvatarTurnOrder Array.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentAvatarTurnIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString CurrentTurnOrderText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool AvatarDiedThisTurn = false;

// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetTurnOrder(const TArray<APlayerController_Base*>& PlayerControllers);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AvatarBeginTurn();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AvatarEndTurn();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EndOfBattle();
};
