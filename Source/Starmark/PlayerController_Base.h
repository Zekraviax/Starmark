#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Actor_CharacterTest.h"
#include "Character_Pathfinder.h"
#include "Engine.h"
#include "UnrealNetwork.h"

#include "PlayerController_Base.generated.h"

// Forward Declarations
class UWidget_HUD_Battle;


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controller")
	E_PlayerCharacter_ClickModes PlayerClickMode;

// ------------------------- Widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Widgets")
	UWidget_HUD_Battle* BattleWidgetReference;

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Pawn")
	ACharacter_Pathfinder* CurrentSelectedAvatar;

// ------------------------- Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Player")
	TArray<FAvatar_Struct> PlayerParty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	bool IsCurrentlyActingPlayer;

// ------------------------- Grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector CursorLocationSnappedToGrid;

// Functions
// --------------------------------------------------

// ------------------------- Widgets
	UFUNCTION(BlueprintCallable)
	void SetBattleWidgetAndLinkedAvatar(UWidget_HUD_Battle* NewBattleWidgetReference, FAvatar_Struct NewAvatarData);

// ------------------------- Avatar
	UFUNCTION(BlueprintCallable)
	void UpdateSelectedAvatar();

// ------------------------- Mouse
	UFUNCTION(BlueprintCallable)
	void OnPrimaryClick(AActor* ClickedActor);

// ------------------------- Battle
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SubtractHealth(ACharacter_Pathfinder* Defender, int DamageDealt);
};