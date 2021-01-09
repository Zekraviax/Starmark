#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Starmark_GameMode.h"
#include "Actor_WorldGridTile.h"

#include "Actor_WorldGrid.generated.h"

// Forward Declarations

// Delegates

// Creation Step Delegate is used in the CreateGrid Function.
DECLARE_DYNAMIC_DELEGATE_TwoParams(FCreationStepDelegate, const FHTileLayout &, TileLayout, const FHCubeCoord &, Coord);


UCLASS()
class STARMARK_API AActor_WorldGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_WorldGrid();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// All of our functions require a category so they show up in BP

	// Create a new grid and fill the CubeCoordinates Array
	UFUNCTION(BlueprintCallable, Category = "Grid", meta = (AutoCreateRefTerm = "CreationStepDelegate"))
	void CreateGrid(const FHTileLayout &TLayout, const int32 GRadius, const FCreationStepDelegate &CreationStepDelegate);

	// Convert Cube Coordinates to World Space
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector TileToWorld(const FHCubeCoord &Tile);

	// Convert World Coordinates to Cube Space
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FHCubeCoord WorldToTile(const FVector &Location);

	// Snap a World Coordinate to Grid Space
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector SnapToGrid(const FVector &Location);

	// Round from floating-point Cube Coordinates to integer Cube Coordinates
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FHCubeCoord GridRound(const FHFractional &F);

	// Compare two Cube Coordinates
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool GridEqual(const FHCubeCoord &A, const FHCubeCoord &B);

	// Return one of the six Cube directions
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FHCubeCoord GetDirection(int32 Direction);

	// Return a neighbour of a Cube Coordinate in the given direciton
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FHCubeCoord GetNeighbour(const FHCubeCoord &Tile, const FHCubeCoord &Direction);

	// Array of Grid Tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TArray<FGridTile> GridTiles;

	// Array of the Cube Coordinates that comprises the Grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TArray<FHCubeCoord> GridCoordinates {};

	// Layout of a Tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FHTileLayout TileLayout {};

	// Radius of the grid in Tiles
	// Clamped between 1 and 25
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridRadius {};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// FHDirections
};
