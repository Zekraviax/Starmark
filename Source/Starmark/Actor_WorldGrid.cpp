#include "Actor_WorldGrid.h"


bool AActor_WorldGrid::IsValidGridCell(const FIntPoint& Location) const
{
	return (Location.X >= 0 && Location.Y >= 0) && (Location.X < MapSize.X && Location.Y < MapSize.Y);
}


bool AActor_WorldGrid::IsGridCellWalkable(const FIntPoint& Location) const
{
	return (!GridTileCoordinates.Contains(Location));
}


bool AActor_WorldGrid::GetGridCellForWorldPos(const FVector& WorldPos, FIntPoint& GridPos) const
{
	FVector MyLocation = GetActorLocation();

	GridPos.X = (WorldPos.X - MyLocation.X) / GridTileSize.X;

	return false;
}