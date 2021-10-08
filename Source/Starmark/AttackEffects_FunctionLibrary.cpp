#include "AttackEffects_FunctionLibrary.h"


#include "Character_Pathfinder.h"



UAttackEffects_FunctionLibrary::UAttackEffects_FunctionLibrary()
{
	//static ConstructorHelpers::FObjectFinder<UDataTable> StatusEffectsDataTableObject(TEXT("DataTable'/Game/DataTables/DT_StatusEffects.DT_StatusEffects'"));

	//if (StatusEffectsDataTableObject.Succeeded())
	//	StatusEffectsDataTable = StatusEffectsDataTableObject.Object;
}



bool UAttackEffects_FunctionLibrary::SwitchOnAttackEffect(EBattle_AttackEffects AttackEffect, ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	FString ContextString;

	//if (LibraryReference->StatusEffectDataTableRows.Num() == 0) {
	//	LibraryReference->StatusEffectDataTableRows = UDataTable::GetAllRows<FAvatar_StatusEffect>(ContextString, LibraryReference->StatusEffectDataTableRows);
	//}

	switch (AttackEffect)
	{
		case (EBattle_AttackEffects::AddParalyzeStatus):
			Attack_AddParalyze(Attacker, Defender);
			break;
		case (EBattle_AttackEffects::AddBurnStatus):
			Attack_AddBurn(Attacker, Defender);
			break;
		default:
			break;
	}

	// Update all players' HUDs and Avatars

	return true;
}


// ------------------------- Status Effect Attacks
void UAttackEffects_FunctionLibrary::Attack_AddParalyze(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	FString ContextString;
	UAttackEffects_FunctionLibrary LibraryReference;

	FAvatar_StatusEffect* ParalyzeStatus = LibraryReference.StatusEffectsDataTable->FindRow<FAvatar_StatusEffect>("Paralyzed", ContextString);
	Defender->CurrentStatusEffectsArray.Add(FAvatar_StatusEffect("Paralyzed", ParalyzeStatus->Image, ParalyzeStatus->TurnsRemaining));
}


void UAttackEffects_FunctionLibrary::Attack_AddBurn(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	FString ContextString;
	UAttackEffects_FunctionLibrary LibraryReference;

	FAvatar_StatusEffect* BurnStatus = LibraryReference.StatusEffectsDataTable->FindRow<FAvatar_StatusEffect>("Burned", ContextString);
	Defender->CurrentStatusEffectsArray.Add(FAvatar_StatusEffect("Burned", BurnStatus->Image, BurnStatus->TurnsRemaining));
}