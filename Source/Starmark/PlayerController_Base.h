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
class AAIController_Avatar;
class UPlayer_SaveData;


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Controller")
	E_PlayerCharacter_ClickModes PlayerClickMode;

// ------------------------- Widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UWidget_HUD_Battle> BattleWidgetChildClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Widgets")
	UWidget_HUD_Battle* BattleWidgetReference;

// ------------------------- Avatar
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRepNotify_CurrentSelectedAvatar, Category = "Avatar")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Avatar")
	ACharacter_Pathfinder* CurrentSelectedAvatar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle PlayerStateTimerHandle;

// ------------------------- Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Player")
	bool IsReadyToStartMultiplayerBattle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Player")
	TArray<FAvatar_Struct> PlayerParty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Player")
	bool IsCurrentlyActingPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Player")
	UPlayer_SaveData* PlayerProfileReference;

// ------------------------- Grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector CursorLocationSnappedToGrid;

// Functions
// --------------------------------------------------

// ------------------------- Widgets
	UFUNCTION(BlueprintCallable)
	void CreateBattleWidget();

	UFUNCTION(BlueprintCallable)
	void SetBattleWidgetVariables();

	UFUNCTION(BlueprintCallable)
	void SetBattleWidgetAndLinkedAvatar(UWidget_HUD_Battle* NewBattleWidgetReference, FAvatar_Struct NewAvatarData);

// ------------------------- Player


// ------------------------- Avatar
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void OnRepNotify_CurrentSelectedAvatar();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void UpdateAvatarsDecalsAndWidgets(ACharacter_Pathfinder* CurrentlyActingAvatar);

// ------------------------- Battle
	UFUNCTION(Server, Reliable)
	void Server_SetReadyToStartMultiplayerBattle();

	UFUNCTION(BlueprintCallable)
	void OnPrimaryClick(AActor* ClickedActor);

	UFUNCTION(BlueprintNativeEvent)
	void SendMoveCommandToServer(FVector MoveLocation);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SendEndOfTurnCommandToServer();
};