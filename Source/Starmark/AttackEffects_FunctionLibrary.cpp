#include "AttackEffects_FunctionLibrary.h"


#include "Character_Pathfinder.h"



bool UAttackEffects_FunctionLibrary::SwitchOnAttackEffect(EBattle_AttackEffects AttackEffect, ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	switch (AttackEffect)
	{
		case (EBattle_AttackEffects::AddParalyzeStatus):
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
}


void UAttackEffects_FunctionLibrary::Attack_AddBurn(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	Defender->CurrentStatusEffectsArray.Add(FAvatar_StatusEffect("Burned", nullptr, 3));
}