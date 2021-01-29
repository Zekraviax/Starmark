#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Starmark_GameMode.h"

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
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

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
	FAvatar_Struct AvatarData;

// ------------------------- Battle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int CurrentHealthPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FAvatar_AttackStruct CurrentSelectedAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	TArray<ACharacter_Pathfinder*> ValidAttackTargetsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	UDataTable* UltimateTypeChartDataTable;

// ------------------------- Other
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Other")
	APlayerController_Base* PlayerControllerReference;

// Functions
// --------------------------------------------------

// ------------------------- Base
	UFUNCTION(BlueprintCallable)
	void BeginPlayWorkaroundFunction();

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
};
