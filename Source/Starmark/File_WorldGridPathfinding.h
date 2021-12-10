#pragma once

#include "CoreMinimal.h"
#include "GraphAStar.h"

#include "Actor_WorldGrid.h"


//class STARMARK_API WorldGridGraphQueryFiler
//{
//public:
//	// Constructor ???
//	WorldGridGraphQueryFiler() : WorldGridReference(nullptr), AllowPartialPaths(false) {}
//
//private:
//// Variables
//// --------------------------------------------------
//
//// ------------------------- 
//	AActor_WorldGrid* WorldGridReference;
//
//	bool AllowPartialPaths;
//
//public:
//// Functions
//// --------------------------------------------------
//
//// ------------------------- 
//	UFUNCTION()
//	void SetWorldGrid(AActor_WorldGrid* SetWorldGridReference);
//
//	UFUNCTION()
//	float GetHeuristicScale() const;
//
//	UFUNCTION()
//	float GetHeuristicCost(FIntPoint StartLocation, FIntPoint EndLocation) const;
//
//	UFUNCTION()
//	float GetTraversalCost(FIntPoint StartLocation, FIntPoint EndLocation) const;
//
//	UFUNCTION()
//	bool IsTraversalAllowed(const FIntPoint FirstPoint, const FIntPoint SecondPoint) const;
//
//	UFUNCTION()
//	bool WantsPartialSolution() const;
//
//	UFUNCTION()
//	void SetAllowPartialPaths(bool InPartialPaths);
//};


class STARMARK_API WorldGridGraphQueryFilter
{
public:
	WorldGridGraphQueryFilter()
		: WorldGrid(nullptr)
		, AllowPartialPaths(false)
	{}

	void SetWorldGrid(AActor_WorldGrid* InWorldGrid)
	{
		WorldGrid = InWorldGrid;
	}

	float GetHeuristicScale() const
	{
		return 1;
	}

	float GetHeuristicCost(const FIntPoint StartNodeRef, const FIntPoint EndNodeRef) const
	{
		return FMath::Abs(StartNodeRef.X - EndNodeRef.X) + FMath::Abs(StartNodeRef.Y - EndNodeRef.Y);
	}

	float GetTraversalCost(const FIntPoint StartNodeRef, const FIntPoint EndNodeRef) const
	{
		return 1;
	}

	bool IsTraversalAllowed(const FIntPoint NodeA, const FIntPoint NodeB) const
	{
		if (!WorldGrid->IsGridCellWalkable(NodeA) || !WorldGrid->IsGridCellWalkable(NodeB))
			return false;

		static const FIntPoint kXOffset(1, 0);
		static const FIntPoint kYOffset(0, 1);

		if (NodeB.X < NodeA.X)
		{
			if (NodeB.Y < NodeA.Y)
			{
				if (!WorldGrid->IsGridCellWalkable(NodeA - kXOffset) || !WorldGrid->IsGridCellWalkable(NodeA - kYOffset))
					return false;
			}
			else if (NodeB.Y > NodeA.Y)
			{
				if (!WorldGrid->IsGridCellWalkable(NodeA - kXOffset) || !WorldGrid->IsGridCellWalkable(NodeA + kYOffset))
					return false;
			}
		}
		else if (NodeB.X > NodeA.X)
		{
			if (NodeB.Y < NodeA.Y)
			{
				if (!WorldGrid->IsGridCellWalkable(NodeA + kXOffset) || !WorldGrid->IsGridCellWalkable(NodeA - kYOffset))
					return false;
			}
			else if (NodeB.Y > NodeA.Y)
			{
				if (!WorldGrid->IsGridCellWalkable(NodeA + kXOffset) || !WorldGrid->IsGridCellWalkable(NodeA + kYOffset))
					return false;
			}
		}

		return true;
	}

	bool WantsPartialSolution() const
	{
		return AllowPartialPaths;
	}

	void SetAllowPartialPaths(bool allow)
	{
		AllowPartialPaths = allow;
	}

private:
	AActor_WorldGrid* WorldGrid;
	bool AllowPartialPaths;
};


class STARMARK_API WorldGridGraph
{

private:
// Variables
// --------------------------------------------------

// ------------------------- 
	UPROPERTY()
	AActor_WorldGrid* WorldGridReference;

public:
	typedef FIntPoint FNodeRef;

// Functions
// --------------------------------------------------

// ------------------------- 
	void SetWorldGrid(AActor_WorldGrid* InWorldGrid);

	int32 GetNeighbourCount(FNodeRef NodeRef) const;

	bool IsValidRef(FNodeRef NodeRef) const;

	FNodeRef GetNeighbour(const FNodeRef NodeRef, const int32 NeighbourIndex) const;
};
