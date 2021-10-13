#include "Actor_AttackEffectsLibrary.h"


#include "Character_Pathfinder.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AActor_AttackEffectsLibrary::AActor_AttackEffectsLibrary()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_AttackEffectsLibrary::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_AttackEffectsLibrary::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Functions
// --------------------------------------------------
void AActor_AttackEffectsLibrary::SwitchOnAttackEffect_Implementation(EBattle_AttackEffects AttackEffect, ACharacter_Pathfinder* Attacker, AActor* Target)
{
	FString ContextString;

	switch (AttackEffect)
	{
	case (EBattle_AttackEffects::AddParalyzeStatus):
		if (Cast<ACharacter_Pathfinder>(Target)) {
			Attack_AddParalyze(Attacker, Cast<ACharacter_Pathfinder>(Target));
		}
		break;
	case (EBattle_AttackEffects::AddBurnStatus):
		if (Cast<ACharacter_Pathfinder>(Target)) {
			Attack_AddParalyze(Attacker, Cast<ACharacter_Pathfinder>(Target));
		}
		break;
	case (EBattle_AttackEffects::KnockbackTarget):
		if (Cast<ACharacter_Pathfinder>(Target)) {
			Attack_AddParalyze(Attacker, Cast<ACharacter_Pathfinder>(Target));
		}
		break;
	default:
		break;
	}
}


// ------------------------- Status Effect Attacks
void AActor_AttackEffectsLibrary::Attack_AddParalyze_Implementation(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	FString ContextString;

	FAvatar_StatusEffect* ParalyzeStatus = StatusEffectsDataTable->FindRow<FAvatar_StatusEffect>("Paralyzed", ContextString);
	Defender->CurrentStatusEffectsArray.Add(FAvatar_StatusEffect("Paralyzed", ParalyzeStatus->Image, ParalyzeStatus->TurnsRemaining));
}


void AActor_AttackEffectsLibrary::Attack_AddBurn_Implementation(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	FString ContextString;

	FAvatar_StatusEffect* BurnStatus = StatusEffectsDataTable->FindRow<FAvatar_StatusEffect>("Burned", ContextString);
	Defender->CurrentStatusEffectsArray.Add(FAvatar_StatusEffect("Burned", BurnStatus->Image, BurnStatus->TurnsRemaining));
}


void AActor_AttackEffectsLibrary::Attack_KnockbackTarget_Implementation(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	FRotator KnockbackDirection = UKismetMathLibrary::FindLookAtRotation(Attacker->GetActorLocation(), Defender->GetActorLocation());
	FVector KnockbackVector = KnockbackDirection.Vector();

	KnockbackVector.X = KnockbackVector.X * 200;
	KnockbackVector.Y = KnockbackVector.Y * 200;

	KnockbackVector = FVector((KnockbackVector.X + Defender->GetActorLocation().X), (KnockbackVector.Y + Defender->GetActorLocation().Y), Defender->GetActorLocation().Z);

	Defender->SetActorLocation(FVector(KnockbackVector.X * 1, KnockbackVector.Y * 1, Defender->GetActorLocation().Z), true);
}