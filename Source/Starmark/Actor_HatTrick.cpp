#include "Actor_HatTrick.h"


// Sets default values
AActor_HatTrick::AActor_HatTrick()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AActor_HatTrick::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_HatTrick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AActor_HatTrick::SelectHatToHideAvatar()
{

}