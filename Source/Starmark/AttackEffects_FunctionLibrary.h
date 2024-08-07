#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Starmark_GameMode.h"

#include "AttackEffects_FunctionLibrary.generated.h"

// Forward Declarations
class ACharacter_Pathfinder;


UCLASS()
class STARMARK_API UAttackEffects_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//virtual UAttackEffects_FunctionLibrary();

// Functions
// --------------------------------------------------
	UFUNCTION(BlueprintCallable)
	static bool SwitchOnAttackEffect(EBattle_AttackEffects AttackEffect, ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);

// ------------------------- Status Effect Attacks
	UFUNCTION(BlueprintCallable)
	static void Attack_AddParalyze(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);

};
