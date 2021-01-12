#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Actor_CharacterTest.h"

#include "PlayerController_Base.generated.h"


UCLASS()
class STARMARK_API APlayerController_Base : public APlayerController
{
	GENERATED_BODY()

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

public:
	APlayerController_Base();

// Variables
// --------------------------------------------------

// ------------------------- Pawn
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pawn")
	AActor_CharacterTest* CurrentSelectedPawn;

// ------------------------- Grid
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	FVector CursorLocationSnappedToGrid;

// Functions
// --------------------------------------------------

// ------------------------- 

};
