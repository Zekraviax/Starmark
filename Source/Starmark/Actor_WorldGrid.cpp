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


void AActor_WorldGrid::CreateGrid(const FHTileLayout &TLayout, const int32 GRadius, const FCreationStepDelegate &CreationStepDelegate)
{
	int32 Size = 1;
	for (int32 i = 0; i <= GridRadius; i++) {
		Size += (6 * i);
	}

	TileLayout = TLayout;
	GridRadius = GRadius;

	for (int32 q = -GridRadius; q <= GridRadius; q++) {
		// R1
		int32 R1 = FMath::Max(-GridRadius, -q - GridRadius);
		// R2
		int32 R2 = FMath::Min(GridRadius, -q + GridRadius);

		for (int32 r = R1; r <= R2; r++) {
			FHCubeCoord CCord
			{
				FIntVector(q, r, (-q - r))
			};

			GridCoordinates.Add(CCord);
			CreationStepDelegate.ExecuteIfBound(TileLayout, CCord);
		}
	}
}

FVector AActor_WorldGrid::TileToWorld(const FHCubeCoord &Tile)
{
	// Set the layout orientation
	//FHTileOrientation TileOrientation {};

	//if (TileLayout.TileOrientation == EHTileOrientationFlag::FLAT) {
	//	TileOrientation = HFlatTopLayout;
	//}
	//else {
	//	TileOrientation = HPointyOrientation;
	//}

	//float x = ((TileOrientation.f0 * Tile.QRS.X) + (TileOrientation.f1 * Tile.QRS.Y)) * TileLayout.TileSize;
	//float y = ((TileOrientation.f2 * Tile.QRS.X) + (TileOrientation.f3 * Tile.QRS.Y)) * TileLayout.TileSize;
	float x = Tile.QRS.X * TileLayout.TileSize;
	float y = Tile.QRS.X * TileLayout.TileSize;

	return FVector(x + TileLayout.Origin.X, y + TileLayout.Origin.Y, TileLayout.Origin.Z);
}

FHCubeCoord AActor_WorldGrid::WorldToTile(const FVector &Location)
{
	// Set the layout orientation
	FHTileOrientation TileOrientation{};

	if (TileLayout.TileOrientation == EHTileOrientationFlag::FLAT) {
		TileOrientation = HFlatTopLayout;
	}
	else {
		TileOrientation = HPointyOrientation;
	}

	FVector InternalLocation
	{
		FVector((Location.X - TileLayout.Origin.X) / TileLayout.TileSize, (Location.Y - TileLayout.Origin.Y) / TileLayout.TileSize, (Location.Z - TileLayout.Origin.Z))
	};

	float q = ((TileOrientation.b0 * InternalLocation.X) + (TileOrientation.b1 * InternalLocation.Y));
	float r = ((TileOrientation.b2 * InternalLocation.X) + (TileOrientation.b3 * InternalLocation.Y));
	FVector v
	{
		(TileLayout.TileOrientation == EHTileOrientationFlag::FLAT) ? FVector(q, (-q - r), r) : FVector(q, r, (-q - r))
	};

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

FHCubeCoord AActor_WorldGrid::GridRound(const FHFractional & F)
{
	return FHCubeCoord();
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
