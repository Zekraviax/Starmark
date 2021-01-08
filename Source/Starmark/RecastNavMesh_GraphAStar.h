#pragma once

#include "CoreMinimal.h"
#include "NavMesh/RecastNavMesh.h"

#include "GraphAStar.h"

#include "RecastNavMesh_GraphAStar.generated.h"

// Forward Declarations


struct FGridPathFilter
{
	FGridPathFilter(const ARecastNavMesh_GraphAStar &NewNavMeshReference) : NavMeshReference(NewNavMeshReference) {}

	// A multiplier for GetHeuristicCost
	float GetHeuristicScale() const;

	// Estimates the cost from the StartNodeReference to the EndNodeReference
	float GetHeuristicCost(const int32 StartNodeReference, const int32 EndNodeReference) const;

	// Returns the true cost of travelling directly from the StartNodeReference to the EndNodeReference
	float GetTraversalCost(const int32 StartNodeReference, const int32 EndNodeReference) const;

	// Returns whether traversing a given edge is allowed
	bool IsTraversalAllowed(const int32 NodeA, const int32 NodeB) const;

	// Whether we accept solutions that don't reach the goal
	bool WantsPartialSolution() const;

protected:
	// A reference to our NavMesh
	const ARecastNavMesh_GraphAStar &NavMeshReference;
};


// Inherit this struct in order to make custom GetCost/GetLength functions
struct FGridNavMeshPath : public FNavMeshPath
{
	FORCEINLINE
	virtual float GetCostFromIndex(int32 PathPointIndex) const override
	{
		return CurrentPathCost;
	}

	FORCEINLINE
		virtual float GetLengthFromPosition(FVector SegmentStart, uint32 NextPathPointIndex) const override
	{
		// Exclude the starting point
		return PathPoints.Num() - 1;
	}

	float CurrentPathCost
	{
		0
	};
};


UCLASS()
class STARMARK_API ARecastNavMesh_GraphAStar : public ARecastNavMesh
{
	GENERATED_BODY()
	
public:
	// Our own implementation of the FindPath function
	// Static because of performance reasons
	// 'Query' is used to find the start and end points of our path
	// The function returns a FPathFindingResult struct that contains the path
	static FPathFindingResult FindPath(const FNavAgentProperties &AgentProperties, const FPathFindingQuery &Query);

	// Set the WorldGrid pointer
	// 'NewWorldGrid' can be null
	UFUNCTION(BlueprintCallable)
	void SetWorldGrid(class AActor_WorldGrid* NewWorldGrid);

	// Define the type we use for Nodes
	typedef int32 FNodeRef;

	// Return the number of neighbours a given NodeReference has
	int32 GetNeighbourCount(FNodeRef NodeReference) const;

	// Return whether a NodeReference is valid
	bool IsValidRef(FNodeRef NodeReference) const;

	// Return a reference to a Node's neighbour
	// (only one neighbour)
	FNodeRef GetNeighbour(const FNodeRef NodeReference, const int32 NeighbourIndex) const;

	// Pointer to a Grid actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AActor_WorldGrid* WorldGrid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float PathPointZOffset
	{
		0.f
	};
};