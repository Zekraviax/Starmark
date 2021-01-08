#pragma once

#include "CoreMinimal.h"
#include "NavMesh/RecastNavMesh.h"
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
	const ARecastNavMesh_GraphAStar NavMeshReference;
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

	// Define the type we use for Nodes
	typedef int32 FNodeReference;

	// Return the number of neighbours a given NodeReference has
	int32 GetNeightbourCount(FNodeReference NodeReference) const;

	// Return whether a NodeReference is valid
	bool IsValidReference(FNodeReference NodeReference) const;

	// Return a reference to a Node's neighbour
	// (only one neighbour)
	FNodeReference GetNeighbour(const FNodeReference NodeReference, const int32 NeighbourIndex) const;

	// Pointer to a Grid actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AActor_WorldGrid* WorldGrid;

	// ---------

	// Set the WorldGrid pointer
	// 'NewWorldGrid' can be null
	UFUNCTION(BlueprintCallable)
	void SetWorldGrid(class AActor_WorldGrid* NewWorldGrid);
};