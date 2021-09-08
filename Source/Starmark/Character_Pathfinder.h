#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Starmark_Variables.h"
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


public:
	ACharacter_Pathfinder();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDecalComponent* ActorSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInstanceDynamic* ActorSelected_DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	FLinearColor ActorSelected_DynamicMaterial_Colour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent* AvatarBattleData_Component;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<UWidgetComponent_AvatarBattleData> AvatarBattleDataComponent_Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent_AvatarBattleData* AvatarBattleDataComponent_Reference;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	//USkeletalMesh* SkeletalMeshReference;

// ------------------------- Avatar
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
	//FDataTableRowHandle AvatarDataTableValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Avatar")
	FAvatar_Struct AvatarData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
	FAvatar_ElementalEssences ElementalEssences;

// ------------------------- Battle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	UDataTable* UltimateTypeChartDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Battle")
	TArray<FAvatar_AttackStruct> CurrentKnownAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Battle")
	FAvatar_AttackStruct CurrentSelectedAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TArray<ACharacter_Pathfinder*> ValidAttackTargetsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TArray<FAvatar_StatusEffect> CurrentStatusEffectsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Battle")
	int IndexInPlayerParty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int MultiplayerControllerUniqueID;

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

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void LaunchAttack(ACharacter_Pathfinder* Target);

	UFUNCTION(BlueprintCallable)
	void SetTilesOccupiedBySize();

	UFUNCTION(BlueprintCallable)
	void UpdateAvatarDataInPlayerParty();

	UFUNCTION(BlueprintCallable)
	void AvatarBeginTurn();

	UFUNCTION()
	void ActorSelectedDynamicMaterialColourUpdate();
};