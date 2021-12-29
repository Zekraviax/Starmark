#include "Actor_StatusEffectsLibrary.h"


#include "Character_Pathfinder.h"


// Sets default values
AActor_StatusEffectsLibrary::AActor_StatusEffectsLibrary()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_StatusEffectsLibrary::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_StatusEffectsLibrary::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Functions
// --------------------------------------------------
void AActor_StatusEffectsLibrary::OnStatusEffectApplied_Implementation(ACharacter_Pathfinder* AffectedAvatar, FAvatar_StatusEffect StatusEffectReference)
{
	// switch expression of type 'const FString' is illegal
	if (StatusEffectReference.Name == FString("Stone Skin")) {
		RememberedVariableOne = FMath::CeilToInt(AffectedAvatar->AvatarData.BaseStats.Defence * 1.5);
		AffectedAvatar->AvatarData.BaseStats.Defence += RememberedVariableOne;
		StatusEffectReference.SpecialFunctionsActor = this;

		AffectedAvatar->CurrentStatusEffectsArray.Add(StatusEffectReference);
	} else {

	}
}


void AActor_StatusEffectsLibrary::OnStatusEffectRemoved_Implementation(ACharacter_Pathfinder* AffectedAvatar, FAvatar_StatusEffect StatusEffectReference)
{
	if (StatusEffectReference.Name == FString("Stone Skin")) {
		AffectedAvatar->AvatarData.BaseStats.Defence -= RememberedVariableOne;
		StatusEffectReference.SpecialFunctionsActor->Destroy();

		AffectedAvatar->CurrentStatusEffectsArray.Remove(StatusEffectReference);
	} else {

	}
}