#include "Actor_WorldGrid.h"

#include "Actor_GridTile.h"
#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController_Battle.h"


bool AActor_WorldGrid::IsValidGridCell(const FIntPoint& Location) const
{
	AActor_GridTile* FoundATile = GetWorldTileActorAtGridCoordinates(Location);

	return (Location.X < MapSize.X && Location.Y < MapSize.Y) && FoundATile;
}


bool AActor_WorldGrid::IsGridCellWalkable(const FIntPoint& Location) const
{
	// Check if the tile has any properties that make it un-traversable
	AActor_GridTile* GridTile = GetWorldTileActorAtGridCoordinates(Location);
	
	if (!IsValid(GridTile)) {
		UE_LOG(LogTemp, Warning, TEXT("AActor_WorldGrid / IsGridCellWalkable / GridTile is not valid"));

		return false;
	}

	// We override this check if the player is checking for valid attack targets
	if (GridTile->Properties.Contains(E_GridTile_Properties::E_Occupied)) {
		if (Cast<APlayerController_Battle>(GetWorld()->GetFirstPlayerController())->PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack) {
			return true;
		}

		return false;
	}

	if (GridTile->Properties.Contains(E_GridTile_Properties::E_Wall)) {
		return false;
	}

	return true;
}

// This function is to be used for pathing only (why??)
// For other world position to tile coordinate conversions, use the other function:
// ConvertGridTileLocationToCoordinates
bool AActor_WorldGrid::ConvertWorldTileToGridCoordinates(const FVector& WorldPos, FIntPoint& GridPos) const
{
	//GetActorLocation() is probably causing both clients and servers to crash in multiplayer
	// The ZeroVector is a temporary solution
	// To-Do: Make this function work in all game modes (singleplayer/multiplayer)
	FVector MyLocation = FVector::ZeroVector;

	GridPos.X = (WorldPos.X - MyLocation.X) / GridTileSize.X;
	GridPos.Y = (WorldPos.Y - MyLocation.Y) / GridTileSize.Y;

	// Returns a bool, but also outputs the grid coordinates
	return (GridPos.X < MapSize.X && GridPos.Y < MapSize.Y);
}


// This function is a more sane version of the previous function:
// ConvertWorldTileToGridCoordinates
FIntPoint AActor_WorldGrid::ConvertGridTileLocationToCoordinates(FVector ActorLocation) const
{
	FIntPoint ReturnCoordinates;

	ReturnCoordinates.X = ActorLocation.X / GridTileSize.X;
	ReturnCoordinates.Y = ActorLocation.Y / GridTileSize.Y;

	return ReturnCoordinates;
}


FVector AActor_WorldGrid::ConvertGridCoordinatesToWorldTile(const FIntPoint& GridCoordinates) const
{
	FVector MyLocation = GetActorLocation();

	return FVector(GridCoordinates.X * GridTileSize.X + MyLocation.X, GridCoordinates.Y * GridTileSize.Y + MyLocation.Y, MyLocation.Z);
}


// To-Do: Check if this function is used anywhere
// (GitHub.dev couldn't find any references)
FVector AActor_WorldGrid::ConvertGridCoordinatesToWorldTileCenter(const FIntPoint& GridCoordinates) const
{
	return ConvertGridCoordinatesToWorldTile(GridCoordinates) + (FVector(GridTileSize.Y, GridTileSize.X, 0) * 0.5f);
}


// This function is to be used for pathing only (why??)
// For other world position to tile coordinate conversions, use the other function:
// FindGridTileAtCoordinates
AActor_GridTile* AActor_WorldGrid::GetWorldTileActorAtGridCoordinates(const FIntPoint& GridCoordinates) const
{
	AActor_GridTile* TileReference = nullptr;
	TArray<AActor*> GridTilesArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	for (int i = 0; i < GridTilesArray.Num(); i++) {
		AActor_GridTile* FoundTile = Cast<AActor_GridTile>(GridTilesArray[i]);
		FIntPoint TileGridCoordinates;

		ConvertWorldTileToGridCoordinates(FoundTile->GetActorLocation(), TileGridCoordinates);

		if (GridCoordinates == TileGridCoordinates) {
			TileReference = FoundTile;
			return TileReference;
		}
	}

	return TileReference;
}


AActor_GridTile* AActor_WorldGrid::FindGridTileAtCoordinates(FIntPoint GridCoordinates)
{
	AActor_GridTile* ReturnTileReference = nullptr;
	FIntPoint TileGridCoordinates;
	TArray<AActor*> GridTilesArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	for (int i = 0; i < GridTilesArray.Num(); i++) {
		AActor_GridTile* FoundTile = Cast<AActor_GridTile>(GridTilesArray[i]);

		TileGridCoordinates = ConvertGridTileLocationToCoordinates(FoundTile->GetActorLocation());

		if (GridCoordinates == TileGridCoordinates) {
			ReturnTileReference = FoundTile;
			return ReturnTileReference;
		}
	}

	return ReturnTileReference;
}


ACharacter_Pathfinder* AActor_WorldGrid::FindCharacterAtCoordinates(FIntPoint GridCoordinates)
{
	ACharacter_Pathfinder* ReturnCharacter = nullptr;
	FIntPoint ActorGridCoordinates;
	TArray<AActor*> CharactersArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), CharactersArray);
	for (int i = 0; i < CharactersArray.Num(); i++) {
		ACharacter_Pathfinder* FoundCharacter = Cast<ACharacter_Pathfinder>(CharactersArray[i]);

		ActorGridCoordinates = ConvertGridTileLocationToCoordinates(FoundCharacter->GetActorLocation());

		if (GridCoordinates == ActorGridCoordinates) {
			ReturnCharacter = FoundCharacter;
			return ReturnCharacter;
		}
	}

	return ReturnCharacter;
}


void AActor_WorldGrid::DrawStraightPathBetweenTwoPositionsWithoutNavigation(FVector PositionOne, FVector PositionTwo, TArray<AActor_GridTile*> &OutGridTilesInPath, TArray<FVector> &OutPositionsInPath)
{
	//TArray<FVector> CoordinatesOnPath;
	//TArray<AActor_GridTile*> GridTilesOnPath;
	FString Direction = "";

	// Step zero, add the first co-ordinates to the returned arrays
	OutPositionsInPath.Add(PositionOne);
	//GridTilesOnPath.Add(FindGridTileAtCoordinates(PositionOne)); // testing this

	// First, figure out which direction to draw the path
	if (PositionOne.X >= PositionTwo.X && PositionOne.Y >= PositionTwo.Y) {
		// South
		Direction = "South";
	} else if (PositionOne.X <= PositionTwo.X && PositionOne.Y <= PositionTwo.Y) {
		// North
		Direction = "North";
	} else if (PositionOne.X >= PositionTwo.X && PositionOne.Y <= PositionTwo.Y) {
		// West
		Direction = "West";
	} else if (PositionOne.X <= PositionTwo.X && PositionOne.Y >= PositionTwo.Y) {
		// East
		Direction = "East";
	}

	// Second, calculate the length of the path
	// Get each co-ordinate and divide by 200
	int X1 = FMath::RoundHalfFromZero(PositionOne.X / 200); 
	int X2 = FMath::RoundHalfFromZero(PositionTwo.X / 200); 
	int Y1 = FMath::RoundHalfFromZero(PositionOne.Y / 200); 
	int Y2 = FMath::RoundHalfFromZero(PositionTwo.Y / 200); 

	// Then subtract X co-ordinates from Y co-ordinates
	// Then get the absolute value of the results
	int XLength = abs(X1 - X2);
	int YLength = abs(Y1 - Y2);

	// Then add them together
	int TotalLength = XLength + YLength;

	// Finally, get tiles and positions in the path one at a time
	for (int i = 0; i < TotalLength; i++) {
		if (Direction == "North") {
			OutPositionsInPath.Add(FVector(OutPositionsInPath[i].X + 200, OutPositionsInPath[i].Y, 0));
			OutGridTilesInPath.Add(FindGridTileAtCoordinates(ConvertGridTileLocationToCoordinates(OutPositionsInPath[i + 1])));  // testing this
		} else if (Direction == "South") {
			OutPositionsInPath.Add(FVector(OutPositionsInPath[i].X - 200, OutPositionsInPath[i].Y, 0));
		} else if (Direction == "East") {
			OutPositionsInPath.Add(FVector(OutPositionsInPath[i].X, OutPositionsInPath[i].Y + 200, 0));
		} else if (Direction == "West") {
			OutPositionsInPath.Add(FVector(OutPositionsInPath[i].X, OutPositionsInPath[i].Y - 200, 0));
		}
	}
}