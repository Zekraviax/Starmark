#include "Actor_WorldGrid.h"

// Sets default values
AActor_WorldGrid::AActor_WorldGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileLayout.TileSize = 102;
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

	TileLayout.TileSize = 102;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("TileLayout.TileSize: %d"), (TLayout.TileSize)));

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
	float x = Tile.QRS.X * TileLayout.TileSize;
	float y = Tile.QRS.Y * TileLayout.TileSize;

	//float x = Tile.QRS.X * 102;
	//float y = Tile.QRS.Y * 102;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Tile X: %d / Tile Y: %d /TileLayout.TileSize: %d"), (Tile.QRS.X), (Tile.QRS.Y), (TileLayout.TileSize)));

	return FVector(x + TileLayout.Origin.X, y + TileLayout.Origin.Y, TileLayout.Origin.Z);
}

FHCubeCoord AActor_WorldGrid::WorldToTile(const FVector &Location)
{
	//FVector InternalLocation
	//{
	//	FVector((Location.X - TileLayout.Origin.X) / TileLayout.TileSize, (Location.Y - TileLayout.Origin.Y) / TileLayout.TileSize, (Location.Z - TileLayout.Origin.Z))
	//};

	//float q = ((InternalLocation.X) + (InternalLocation.Y));
	//float r = ((InternalLocation.X) + (InternalLocation.Y));
	//FVector v = FVector(q, r, 1);

	//{
	//	(TileLayout.TileOrientation == EHTileOrientationFlag::FLAT) ? FVector(q, (-q - r), r) : FVector(q, r, (-q - r))
	//};

	float x = (Location.X - TileLayout.Origin.X) / TileLayout.TileSize;
	float y = (Location.Y - TileLayout.Origin.Y) / TileLayout.TileSize;

	FHCubeCoord ReturnCoords = FHCubeCoord(x, y, TileLayout.Origin.Z);

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("X: %d / TileLayout.Orgin.X: %d / TileLayout.TileSize: %d"), x, TileLayout.Origin.X, TileLayout.TileSize));

	return ReturnCoords;
}

FVector AActor_WorldGrid::SnapToGrid(const FVector &Location)
{
	float TempZ = Location.Z;

	//FVector Result
	//{
	//	TileToWorld(WorldToTile(Location))
	//};

	FHCubeCoord Result1 = WorldToTile(Location);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("WorldToTile.X: %d / WorldToTile.Y: %d / WorldToTile.Z: %d"), Result1.QRS.X, Result1.QRS.Y, Result1.QRS.Z));
	FVector Result = TileToWorld(Result1);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("TileToWorld.X: %d / TileToWorld.Y: %d / TileToWorld.Z: %d"), Result.X, Result.Y, Result.Z));

	Result.Z = TempZ;
	return Result;
}

FHCubeCoord AActor_WorldGrid::GridRound(const FHFractional & F)
{
	int32 q
	{
		int32(FMath::RoundToDouble(F.QRS.X)) 
	};
	int32 r
	{ 
		int32(FMath::RoundToDouble(F.QRS.Y)) 
	};
	int32 s
	{ 
		int32(FMath::RoundToDouble(F.QRS.Z)) 
	};

	float q_diff{ FMath::Abs(q - F.QRS.X) };
	float r_diff{ FMath::Abs(r - F.QRS.Y) };
	float s_diff{ FMath::Abs(s - F.QRS.Z) };

	if ((q_diff > r_diff) && (q_diff > s_diff))
	{
		q = -r - s;
	}
	else if (r_diff > s_diff)
	{
		r = -q - s;
	}
	else
	{
		s = -q - r;
	}

	return FHCubeCoord{ FIntVector(q, r, s) };
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
