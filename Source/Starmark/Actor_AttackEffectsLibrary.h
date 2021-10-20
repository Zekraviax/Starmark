#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Engine/DataTable.h"
#include "Starmark_Variables.h"

#include "Actor_AttackEffectsLibrary.generated.h"


// Forward Declarations
class AActor_GridTile;
class ACharacter_Pathfinder;


UCLASS()
class STARMARK_API AActor_AttackEffectsLibrary : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_AttackEffectsLibrary();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// Variables
// --------------------------------------------------

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACharacter_Pathfinder> RockWall_Class;

// ------------------------- Data Tables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* StatusEffectsDataTable;

// Functions
// --------------------------------------------------
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void SwitchOnAttackEffect(EBattle_AttackEffects AttackEffect, ACharacter_Pathfinder* Attacker, AActor* Target);

// ------------------------- Status Effects
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Attack_AddParalyze(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Attack_AddBurn(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Attack_AddDrowning(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);

// ------------------------- Other
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Attack_KnockbackTarget(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Spawn_RockWall(AActor_GridTile* TargetTile);
};
