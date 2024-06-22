#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"


#include "Actor_WorldGrid.generated.h"


// Forward Declarations
class AActor_GridTile;
class ACharacter_Pathfinder;


UCLASS()
class STARMARK_API AActor_WorldGrid : public AActor
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FIntPoint> GridTileCoordinates;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIntPoint MapSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D GridTileSize;

// Functions
// --------------------------------------------------

// ------------------------- 
	// Why are these marked const? Because they should never modify the variables passed to them.
	// This is enforced at compilation
	bool IsValidGridCell(const FIntPoint& Location) const;
	bool IsGridCellWalkable(const FIntPoint& Location) const;

	UFUNCTION(BlueprintCallable)
	bool ConvertWorldTileToGridCoordinates(const FVector& WorldPos, FIntPoint& GridPos) const;

	UFUNCTION(BlueprintCallable)
	FIntPoint ConvertGridTileLocationToCoordinates(const FVector ActorLocation) const;

	UFUNCTION(BlueprintCallable)
	FVector ConvertGridCoordinatesToWorldTile(const FIntPoint& GridCoordinates) const;

	UFUNCTION(BlueprintCallable)
	FVector ConvertGridCoordinatesToWorldTileCenter(const FIntPoint& GridCoordinates) const;

	// The following functions are examples of overloaded functions
	// Don't forget that they still have unique signatures, and thus require their own
	// function definitions in the corresponding .ccp file
	AActor_GridTile* GetAndReturnGridTileAtLocation(const FIntPoint GridCoordinates) const;
	AActor_GridTile* GetAndReturnGridTileAtLocation(const FVector Position) const;

	ACharacter_Pathfinder* GetAndReturnCharacterAtLocation(const FIntPoint GridCoordinates) const;
	ACharacter_Pathfinder* GetAndReturnCharacterAtLocation(const FVector Position) const;

	// Shouldn't be used with any curving paths (yet)
	void DrawStraightPathBetweenTwoPositionsWithoutNavigation(FVector PositionOne, FVector PositionTwo, TArray<AActor_GridTile*> &OutGridTilesInPath, TArray<FVector> &OutPositionsInPath);
};