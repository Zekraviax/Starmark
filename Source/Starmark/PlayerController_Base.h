#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Actor_CharacterTest.h"
#include "Character_Pathfinder.h"

#include "PlayerController_Base.generated.h"

// Forward Declarations
//class ACharacter_Pathfinder;

// Unique Enums
UENUM(BlueprintType)
enum class E_PlayerCharacter_ClickModes : uint8
{
	E_Nothing,
	E_SelectCharacterToControl,
	E_SelectCharacterToAttack,
	E_MoveCharacter,
};


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

// ------------------------- Controller
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Controller")
	E_PlayerCharacter_ClickModes PlayerClickMode;

// ------------------------- Pawn
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pawn")
	ACharacter_Pathfinder* CurrentSelectedAvatar;

// ------------------------- Grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector CursorLocationSnappedToGrid;

// ------------------------- Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TArray<FAvatar_Struct> PlayerParty;

// Functions
// --------------------------------------------------

// ------------------------- Base
	UFUNCTION(BlueprintCallable)
	void SetRandomPawnAsSelectedPawn(ACharacter_Pathfinder* RandomPawnReference);

	UFUNCTION(BlueprintCallable)
	void UpdateSelectedAvatar();

// ------------------------- Mouse
	UFUNCTION(BlueprintCallable)
	void OnPrimaryClick(AActor* ClickedActor);

	UFUNCTION()
	void UpdateCursorSelectedMaterial();
};
