#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Starmark_Variables.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"

#include "Character_Pathfinder.generated.h"


// Forward Declarations
class AActor_GridTile;
class APlayerController_Battle;
class UWidget_HUD_Battle;
class UWidgetComponent_AvatarBattleData;


UENUM(BlueprintType)
enum class ECharacter_FacingDirections : uint8
{
	TopRight,
	TopLeft,
	BottomRight,
	BottomLeft
};


UCLASS()
class STARMARK_API ACharacter_Pathfinder : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacter_Pathfinder();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* ActorHighlightedDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* ActorHighlightMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* ActorHighlightedDecalMaterialInstanceDynamic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ActorSelectedPlane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor ActorSelectedMaterialInstanceDynamicColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* AvatarBattleData_Component;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_AvatarBattleData> AvatarBattleDataComponent_Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent_AvatarBattleData* AvatarBattleDataComponent_Reference;

// ------------------------- Entity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FAvatar_Struct AvatarData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RotateAvatarTowardsMouse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle EntityDataTableRow;

// ------------------------- Battle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* UltimateTypeChartDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<FAvatar_AttackStruct> CurrentKnownAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FAvatar_AttackStruct CurrentSelectedAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> ValidAttackTargetsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAvatar_StatusEffect> CurrentStatusEffectsArray;

	// This variable ties this physical avatar actor to a FAvatar struct in the GameState array, it shouldn't be used for anything else.
	// We can't do pass-by-reference for this value because I don't know how to initialize the variable lol.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int AvatarBattleUniqueID;
	//Cast<AStarmark_GameState>(AActor::GetWorld()->GetGameState())->PlayerDataStructsArray[0].CurrentAvatarTeam[0].BattleUniqueID
	
	// This variable ties this physical avatar actor to a player controller, it shouldn't be used for anything else.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MultiplayerControllerUniqueID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* AttackTraceActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> AttackTraceStaticMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackRotationSnapToDegrees = 90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor_GridTile*> CurrentPathTiles;

// ------------------------- Other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	APlayerController_Battle* PlayerControllerReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FPlayer_Data ControllingPlayerDataCopy;

	UPROPERTY()
	FString CharacterContextString;

// Functions
// --------------------------------------------------

// ------------------------- Base
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void BeginPlayWorkaroundFunction(UWidget_HUD_Battle* BattleHUDReference = nullptr);

// ------------------------- Cursor
	UFUNCTION(BlueprintCallable)
	void OnAvatarCursorOverBegin();

	UFUNCTION(BlueprintCallable)
	void OnAvatarCursorOverEnd();

	UFUNCTION(BlueprintCallable)
	void OnAvatarClicked();

// ------------------------- Battle
	UFUNCTION(BlueprintCallable)
	void SetActorHighlightProperties(bool IsVisible, E_GridTile_ColourChangeContext ColourChangeContext);

	// Used to highlight valid actors when an attack is selected but not confirmed.
	// Used to get valid targets when confirming an attack.
	// Returns an array of actors.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetValidActorsForAttack(FAvatar_AttackStruct Attack, AActor* CurrentlyHoveredActor);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void LaunchAttack(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void SetTilesOccupiedBySize(bool ClearTiles);

	UFUNCTION(BlueprintCallable)
	void UpdateAvatarDataInPlayerParty();

	UFUNCTION(BlueprintNativeEvent)
	void ActorSelectedDynamicMaterialColourUpdate();

	UFUNCTION(BlueprintNativeEvent)
	void AvatarBeginTileOverlap();

	UFUNCTION(BlueprintCallable)
	void AvatarStopMoving(bool SnapToGrid);

// ------------------------- Multiplayer
	UFUNCTION(Client, Unreliable)
	void Client_GetAvatarData(FAvatar_Struct NewAvatarData);
	void Local_GetAvatarData(const FAvatar_Struct& NewAvatarData);

	UFUNCTION(BlueprintCallable)
	ECharacter_FacingDirections GetCharacterFacingDirection();

	UFUNCTION(Server, Reliable)
	void SynchronizeAvatarDataToGameState();
};