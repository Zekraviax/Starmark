#include "Actor_WorldGrid.h"

// Sets default values
AActor_WorldGrid::AActor_WorldGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileLayout.TileSize = 100;
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
	//int32 Size = 1;
	//for (int32 i = 0; i <= GridRadius; i++) {
	//	Size += (6 * i);
	//}

	TileLayout = TLayout;
	GridRadius = GRadius;

	TileLayout.TileSize = 100;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("TileLayout.TileSize: %d"), (TLayout.TileSize)));

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
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Emerald, FString::Printf(TEXT("Tile To World: %s"), *Tile.QRS.ToString()));

	float x = Tile.QRS.X * TileSize;
	float y = Tile.QRS.Y * TileSize;

	TileLayout.Origin.X = 1;
	TileLayout.Origin.Y = 1;
	TileLayout.Origin.Z = 1;

	return FVector(x + 1, y + 1, 1);
}

FHCubeCoord AActor_WorldGrid::WorldToTile(const FVector &Location)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString::Printf(TEXT("World to Tile: %s"), *Location.ToString()));

	TileLayout.Origin.X = 1;
	TileLayout.Origin.Y = 1;
	TileLayout.Origin.Z = 1;

	float x = (Location.X - 1) / TileSize;
	float y = (Location.Y - 1) / TileSize;

	FHCubeCoord ReturnCoords = FHCubeCoord(x, y, TileLayout.Origin.Z);

	return ReturnCoords;
}

FVector AActor_WorldGrid::SnapToGrid(const FVector &Location)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Magenta, FString::Printf(TEXT("Original Location: %s"), *Location.ToString()));

	FHCubeCoord Result1 = WorldToTile(Location);
	FVector Result = TileToWorld(Result1);

	Result.Z = Location.Z;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Result: %s"), *Result.ToString()));
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
