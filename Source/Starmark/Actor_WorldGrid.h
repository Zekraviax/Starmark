#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"


#include "Actor_WorldGrid.generated.h"


// Forward Declarations
class AActor_GridTile;
class ACharacter_Pathfinder;



// A* Node Struct
USTRUCT(BlueprintType)
struct STARMARK_API FAStarNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position;

	// f = g + h
	// f is the total cost of the node
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalCost;

	// g is the distance between the start node and the current node
	// this is the G value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DistanceFromStartNode; 

	// h is the heuristic
	// which means the estimated distance between the current node and the end node
	// we're going to use the pythagorean equation to get this value
	// a²+ b² = c²
	// a is the east/west distance to the end
	// b is the north/south distance to the end
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EstimatedDistanceToEndNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ParentNodePosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsValid = false;

	FAStarNode() {}	// default constructor

	// custom constructor
	FAStarNode(FVector inPosition, int inTotalCost = 0, int inDistanceFromStartNode = 0, int inEstimatedDistanceToEndNode = 0)
	{
		Position = inPosition;
		TotalCost = inTotalCost;
		DistanceFromStartNode = inDistanceFromStartNode;
		EstimatedDistanceToEndNode = inEstimatedDistanceToEndNode;
	}

	// To-Do: create a comparator override function here that compares the positions
	// maybe create a second comparator that compares all 4 values
	FORCEINLINE bool operator==(const FAStarNode& SecondNode) const
	{
		return FMath::IsNearlyEqual(Position.X, SecondNode.Position.X, 0.1f) &&
			FMath::IsNearlyEqual(Position.Y, SecondNode.Position.Y, 0.1f);
	}
};


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

	// The following functions are examples of overloaded functions
	// Don't forget that they still have unique signatures, and thus require their own
	// function definitions in the corresponding .cpp file
	AActor_GridTile* GetAndReturnGridTileAtLocation(const FIntPoint& GridCoordinates) const;
	AActor_GridTile* GetAndReturnGridTileAtLocation(const FVector Position) const;

	ACharacter_Pathfinder* GetAndReturnCharacterAtLocation(const FIntPoint GridCoordinates) const;
	ACharacter_Pathfinder* GetAndReturnCharacterAtLocation(const FVector Position) const;

	// Shouldn't be used with any curving paths (yet)
	static int GetTotalDistanceBetweenTwoPositions(FVector PositionOne, FVector PositionTwo);
	void DrawStraightPathBetweenTwoPositionsWithoutNavigation(FVector PositionOne, FVector PositionTwo, TArray<AActor_GridTile*> &OutGridTilesInPath, TArray<FVector> &OutPositionsInPath);

	// aaaaaAAAAAAAAAAAA
	void CalculateValuesForAStarNode(FAStarNode& Node, const FAStarNode StartNode, const FAStarNode EndNode); 	// (don't use this function for the start and end nodes)
	TArray<FVector> CustomAStarPathfindingAlgorithm(FVector StartPosition, FVector EndPosition);
};