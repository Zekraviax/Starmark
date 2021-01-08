#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Starmark_GameMode.h"
#include "Actor_WorldGridTile.h"

#include "Actor_WorldGrid.generated.h"

// Forward Declarations



UCLASS()
class STARMARK_API AActor_WorldGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_WorldGrid();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Create a new grid and fill the CubeCoordinates Array
	UFUNCTION(BlueprintCallable)
	void CreateGrid(const FHTileLayout &TLayout, const int32 GridRadius);

	// Convert Cube Coordinates to World Space
	UFUNCTION(BlueprintCallable)
	FVector TileToWorld(const FHCubeCoord &Tile);

	// Convert World Coordinates to Cube Space
	UFUNCTION(BlueprintCallable)
	FHCubeCoord WorldToTile(const FVector &Location);

	// Snap a World Coordinate to Grid Space
	UFUNCTION(BlueprintCallable)
	FVector SnapToGrid(const FVector &Location);

	// Round from floating-point Cube Coordinates to integer Cube Coordinates
	//UFUNCTION(BlueprintCallable)
	//FHCubeCoord GridRound(const FHFractional &F)

	// Compare two Cube Coordinates
	UFUNCTION(BlueprintCallable)
	bool GridEqual(const FHCubeCoord &A, const FHCubeCoord &B);

	// Return one of the six Cube directions
	UFUNCTION(BlueprintCallable)
	FHCubeCoord GetDirection(int32 Direction);

	// Return a neighbour of a Cube Coordinate in the given direciton
	UFUNCTION(BlueprintCallable)
	FHCubeCoord GetNeighbour(const FHCubeCoord &Tile, const FHCubeCoord &Direction);

	// Array of Grid Tiles
	UPROPERTY(BlueprintReadWrite)
	TArray<FGridTile> GridTiles;

	// Array of the Cube Coordinates that comprises the Grid
	UPROPERTY(BlueprintReadWrite)
	TArray<FHCubeCoord> GridCoordinates {};

	// Layout of a Tile
	UPROPERTY(BlueprintReadWrite)
	FHTileLayout TileLayout {};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// FHDirections
};
