#include "Actor_WorldGrid.h"

// Sets default values
AActor_WorldGrid::AActor_WorldGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_WorldGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_WorldGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

