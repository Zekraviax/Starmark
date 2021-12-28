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
	if (StatusEffectReference.Name == FString("StoneSkin")) {
		RememberedVariableOne = FMath::RoundToInt(AffectedAvatar->AvatarData.BaseStats.Defence * 1.5);
		AffectedAvatar->AvatarData.BaseStats.Defence += RememberedVariableOne;
		StatusEffectReference.SpecialFunctionsActor = this;
	} else {

	}
}