#include "AttackEffects_FunctionLibrary.h"


#include "Character_Pathfinder.h"



bool UAttackEffects_FunctionLibrary::SwitchOnAttackEffect(EBattle_AttackEffects AttackEffect, ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	switch (AttackEffect)
	{
	case (EBattle_AttackEffects::AddParalyzeStatus):
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Paralyze Target")));
		Attack_AddParalyze(Attacker, Defender);
		break;
	default:
		break;
	}

	return true;
}


// ------------------------- Status Effect Attacks
void UAttackEffects_FunctionLibrary::Attack_AddParalyze(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	Defender->CurrentStatusEffectsArray.Add(FAvatar_StatusEffect("Paralyzed", nullptr, 3));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Paralyze Target")));
}