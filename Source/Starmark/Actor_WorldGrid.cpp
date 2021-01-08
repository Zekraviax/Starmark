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


void AActor_WorldGrid::CreateGrid(const FHTileLayout &TLayout, const int32 GridRadius)
{

}

FVector AActor_WorldGrid::TileToWorld(const FHCubeCoord &Tile)
{
	// Set the layout orientation
	//FHTileOrientation TileOrientation{};
	//if (TileLayout.TileOrientation) {

	//}

	//float x = ((QRS.X));

	return FVector();
}

FHCubeCoord AActor_WorldGrid::WorldToTile(const FVector &Location)
{
	return FHCubeCoord();
}

FVector AActor_WorldGrid::SnapToGrid(const FVector &Location)
{
	float TempZ
	{
		Location.Z
	};

	FVector Result
	{
		TileToWorld(WorldToTile(Location))
	};

	Result.Z = TempZ;
	return Result;
}

bool AActor_WorldGrid::GridEqual(const FHCubeCoord &A, const FHCubeCoord &B)
{
	return A == B;
}

FHCubeCoord AActor_WorldGrid::GetDirection(int32 Direction)
{
	//check(Direction < HDirections.Directions.Num());
	return FHCubeCoord();
}

FHCubeCoord AActor_WorldGrid::GetNeighbour(const FHCubeCoord &Tile, const FHCubeCoord &Direction)
{
	return Tile + Direction;
}
