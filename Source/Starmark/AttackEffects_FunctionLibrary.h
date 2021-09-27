#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Starmark_Variables.h"

#include "AttackEffects_FunctionLibrary.generated.h"


// Forward Declarations
class ACharacter_Pathfinder;


UCLASS()
class STARMARK_API UAttackEffects_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
// Functions
// --------------------------------------------------
	UFUNCTION(BlueprintCallable)
	static bool SwitchOnAttackEffect(EBattle_AttackEffects AttackEffect, ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);

// ------------------------- Status Effects
	UFUNCTION(BlueprintCallable)
	static void Attack_AddParalyze(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);

	UFUNCTION(BlueprintCallable)
	static void Attack_AddBurn(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);
};
