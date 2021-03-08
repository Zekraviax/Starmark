#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Starmark_GameMode.h"
#include "Engine.h"
#include "UnrealNetwork.h"

#include "Character_Pathfinder.generated.h"

// Forward Declarations
class APlayerController_Base;
class UWidgetComponent_AvatarBattleData;


UCLASS()
class STARMARK_API ACharacter_Pathfinder : public ACharacter
{
	GENERATED_BODY()

private:
	///** Top down camera */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UCameraComponent* TopDownCameraComponent;

	///** Camera boom positioning the camera above the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class USpringArmComponent* CameraBoom;



public:
	ACharacter_Pathfinder();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	///** Returns TopDownCameraComponent subobject **/
	//FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	///** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

// Variables
// --------------------------------------------------

// ------------------------- Components
	/** A decal that projects to the cursor location. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDecalComponent* CursorToWorld;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstanceDynamic* CursorToWorld_DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDecalComponent* ActorSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstanceDynamic* ActorSelected_DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent* AvatarBattleData_Component;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<UWidgetComponent_AvatarBattleData> AvatarBattleDataComponent_Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent_AvatarBattleData* AvatarBattleDataComponent_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMesh* SkeletalMeshReference;
	//TSubclassOf<USkeletalMesh> SkeletalMeshReference;

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
	FDataTableRowHandle AvatarDataTableValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Avatar")
	FAvatar_Struct AvatarData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
	FAvatar_ElementalEssences ElementalEssences;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
	int CurrentLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category = "Avatar")
	int CurrentHealthPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category = "Avatar")
	int CurrentManaPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category = "Avatar")
	int CurrentTileMoves;

// ------------------------- Battle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TArray<FAvatar_AttackStruct> AttacksArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Battle")
	TArray<FAvatar_AttackStruct> AllKnownAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FAvatar_AttackStruct CurrentSelectedAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TArray<ACharacter_Pathfinder*> ValidAttackTargetsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	UDataTable* UltimateTypeChartDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TArray<FAvatar_StatusEffect> CurrentStatusEffectsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int IndexInPlayerParty;

// ------------------------- AI
	

// ------------------------- Other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Other")
	APlayerController_Base* PlayerControllerReference;

// Functions
// --------------------------------------------------

// ------------------------- Base
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void BeginPlayWorkaroundFunction(FAvatar_Struct NewAvatarData, UWidget_HUD_Battle* BattleHUDReference = nullptr);

// ------------------------- Cursor
	UFUNCTION(BlueprintCallable)
	void OnAvatarCursorOverBegin();

	UFUNCTION(BlueprintCallable)
	void OnAvatarCursorOverEnd();

	UFUNCTION(BlueprintCallable)
	void OnAvatarClicked();

// ------------------------- Battle
	UFUNCTION(BlueprintCallable)
	void ShowAttackRange();

	UFUNCTION(BlueprintCallable)
	void LaunchAttack(ACharacter_Pathfinder* Target);

	UFUNCTION(BlueprintCallable)
	void SetTilesOccupiedBySize();
};
