#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Actor_CharacterTest.h"
#include "Character_Pathfinder.h"

#include "PlayerController_Base.generated.h"

// Forward Declarations
//class ACharacter_Pathfinder;


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

public:
	APlayerController_Base();

// Variables
// --------------------------------------------------

// ------------------------- Pawn
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pawn")
	ACharacter_Pathfinder* CurrentSelectedPawn;

// ------------------------- Grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector CursorLocationSnappedToGrid;

// Functions
// --------------------------------------------------

// ------------------------- 

};
