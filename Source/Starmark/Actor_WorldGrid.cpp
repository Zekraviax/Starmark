#include "Actor_WorldGrid.h"

#include "Actor_GridTile.h"
#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController_Battle.h"


bool AActor_WorldGrid::IsValidGridCell(const FIntPoint& Location) const
{
	AActor_GridTile* FoundATile = GetAndReturnGridTileAtLocation(Location);

	return (Location.X < MapSize.X && Location.Y < MapSize.Y) && FoundATile;
}


bool AActor_WorldGrid::IsGridCellWalkable(const FIntPoint& Location) const
{
	// Check if the tile has any properties that make it un-traversable
	AActor_GridTile* GridTile = GetAndReturnGridTileAtLocation(Location);
	
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


// This function is a saner version of the previous function:
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


AActor_GridTile* AActor_WorldGrid::GetAndReturnGridTileAtLocation(const FIntPoint& GridCoordinates) const
{
	AActor_GridTile* ReturnTileReference = nullptr;
	TArray<AActor*> GridTilesArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	for (int i = 0; i < GridTilesArray.Num(); i++) {
		AActor_GridTile* FoundTile = Cast<AActor_GridTile>(GridTilesArray[i]);
		FIntPoint TileGridCoordinates = ConvertGridTileLocationToCoordinates(FoundTile->GetActorLocation());

		if (GridCoordinates == TileGridCoordinates) {
			ReturnTileReference = FoundTile;
			break;
		}
	}

	return ReturnTileReference;
}


AActor_GridTile* AActor_WorldGrid::GetAndReturnGridTileAtLocation(const FVector Position) const
{
	AActor_GridTile* ReturnGridTile = nullptr;
	TArray<AActor*> GridTilesArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	for (int i = 0; i < GridTilesArray.Num(); i++) {
		AActor_GridTile* FoundGridTile = Cast<AActor_GridTile>(GridTilesArray[i]);

		FVector GridTilePosition = FoundGridTile->GetActorLocation();

		if (Position.Equals(GridTilePosition, 1.f)) {
			ReturnGridTile = FoundGridTile;
			break;
		}
	}

	return ReturnGridTile;
}


ACharacter_Pathfinder* AActor_WorldGrid::GetAndReturnCharacterAtLocation(const FIntPoint GridCoordinates) const
{
	ACharacter_Pathfinder* ReturnCharacter = nullptr;
	TArray<AActor*> CharactersArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), CharactersArray);
	for (int i = 0; i < CharactersArray.Num(); i++) {
		ACharacter_Pathfinder* FoundCharacter = Cast<ACharacter_Pathfinder>(CharactersArray[i]);

		FIntPoint ActorGridCoordinates = ConvertGridTileLocationToCoordinates(FoundCharacter->GetActorLocation());

		if (GridCoordinates == ActorGridCoordinates) {
			ReturnCharacter = FoundCharacter;
			break;
		}
	}

	return ReturnCharacter;
}

ACharacter_Pathfinder* AActor_WorldGrid::GetAndReturnCharacterAtLocation(const FVector Position) const 
{
	ACharacter_Pathfinder* ReturnCharacter = nullptr;
	TArray<AActor*> CharactersArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), CharactersArray);
	for (int i = 0; i < CharactersArray.Num(); i++) {
		ACharacter_Pathfinder* FoundCharacter = Cast<ACharacter_Pathfinder>(CharactersArray[i]);

		FVector ActorLocation = FoundCharacter->GetActorLocation();
		if (Position.Equals(ActorLocation, 1.f)) {
			ReturnCharacter = FoundCharacter;
			break;
		}
	}

	return ReturnCharacter;
}

int AActor_WorldGrid::GetTotalDistanceBetweenTwoPositions(FVector PositionOne, FVector PositionTwo)
{
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
	return (XLength + YLength);
}



void AActor_WorldGrid::DrawStraightPathBetweenTwoPositionsWithoutNavigation(FVector PositionOne, FVector PositionTwo, TArray<AActor_GridTile*> &OutGridTilesInPath, TArray<FVector> &OutPositionsInPath)
{
	FString Direction = "";

	// Step zero, add the first co-ordinates to the returned arrays
	OutPositionsInPath.Add(PositionOne);

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
	int TotalLength = GetTotalDistanceBetweenTwoPositions(PositionOne, PositionTwo);

	// Finally, get tiles and positions in the path one at a time
	for (int i = 0; i < TotalLength; i++) {
		if (Direction == "North") {
			OutPositionsInPath.Add(FVector(OutPositionsInPath[i].X + 200, OutPositionsInPath[i].Y, 0));
			OutGridTilesInPath.Add(GetAndReturnGridTileAtLocation(ConvertGridTileLocationToCoordinates(OutPositionsInPath[i + 1])));  // testing this
		} else if (Direction == "South") {
			OutPositionsInPath.Add(FVector(OutPositionsInPath[i].X - 200, OutPositionsInPath[i].Y, 0));
		} else if (Direction == "East") {
			OutPositionsInPath.Add(FVector(OutPositionsInPath[i].X, OutPositionsInPath[i].Y + 200, 0));
		} else if (Direction == "West") {
			OutPositionsInPath.Add(FVector(OutPositionsInPath[i].X, OutPositionsInPath[i].Y - 200, 0));
		}
	}
}


void AActor_WorldGrid::CalculateValuesForAStarNode(FAStarNode& Node, const FAStarNode StartNode, const FAStarNode EndNode)
{
	// g = DistanceFromStartNode
	Node.DistanceFromStartNode = GetTotalDistanceBetweenTwoPositions(StartNode.Position, Node.Position);

	// h = EstimatedDistanceToEndNode
	Node.EstimatedDistanceToEndNode = ((Node.Position.X - EndNode.Position.X) * 2) + ((Node.Position.Y - EndNode.Position.Y) * 2);

	// f = g + h
	Node.TotalCost = Node.DistanceFromStartNode + Node.EstimatedDistanceToEndNode;
}


TArray<FVector> AActor_WorldGrid::CustomAStarPathfindingAlgorithm(FVector StartPosition, FVector EndPosition)
{
	// Step one: add the start position to the Open List
	StartPosition.X = FMath::RoundHalfFromZero(StartPosition.X);
	StartPosition.Y = FMath::RoundHalfFromZero(StartPosition.Y);
	StartPosition.Z = 0;
	FAStarNode StartNode = FAStarNode(StartPosition);

	EndPosition.X = FMath::RoundHalfFromZero(EndPosition.X);
	EndPosition.Y = FMath::RoundHalfFromZero(EndPosition.Y);
	EndPosition.Z = 0;
	FAStarNode EndNode = FAStarNode(EndPosition);

	// leave the start node's F value at zero
	TArray<FAStarNode> OpenList = { StartNode };
	TArray<FAStarNode> ClosedList;
	FAStarNode CurrentNode;

	TArray<FVector> Path;

	// Loop through the next steps for as long as the end position has not been reached and the open list is not empty
		// A. find the lowest cost node in the open list
		// make that node the current node

		// B. take that node from the open list and put it in to the closed list

		// C. for each node adjacent to the current node, check the following criteria:
			// if the node is in the closed list, ignore it
			// if the node cannot be walked on, ignore it

			// if the node isn't in the open list, add it to the open list...
			//... and make the current node the parent of this node...
			//... and record the F, G, and H values of this node

		// D. Stop when the target node is found, or the target node cannot be found

	while (OpenList.Num() > 0) {
		CurrentNode = OpenList[0];

		// find the open node with the lowest total cost (F value)
		for (int i = 0; i < OpenList.Num(); i++) {
			if (OpenList[i].TotalCost < CurrentNode.TotalCost) {
				CurrentNode = OpenList[i];
			}
		}

		// Remove the current node from the open list and add it to the closed list.
		OpenList.Remove(CurrentNode);
		ClosedList.Add(CurrentNode);

		// The pathfinding is complete if this is true.
		if (CurrentNode == EndNode) {
			// Here we "draw" the path, starting from the end and working backwards.
			for (int i = ClosedList.Num() - 1; i >= 0; i--) {
				Path.Add(ClosedList[i].Position);
			}
			
			return Path;
		}

		// Get all the adjacent nodes of the current node.
		// These nodes are the "children" of the current node.
		TArray<FAStarNode> ChildrenNodes = { FAStarNode(FVector(CurrentNode.Position.X + 200, CurrentNode.Position.Y, 0)), 
		FAStarNode(FVector(CurrentNode.Position.X, CurrentNode.Position.Y + 200, 0)),
		FAStarNode(FVector(CurrentNode.Position.X - 200, CurrentNode.Position.Y, 0)),
		FAStarNode(FVector(CurrentNode.Position.X, CurrentNode.Position.Y - 200, 0)), };

		// it's here in this if/else statement that we can check for other conditions like whether or not the node can be traversed.
		for (FAStarNode ChildNode : ChildrenNodes) {
			// Check if the child node is in the closed list.
			if (ClosedList.Contains(ChildNode)) {
				// If this is true, just ignore the child.
				continue;
			}
			
			if (!IsGridCellWalkable(ConvertGridTileLocationToCoordinates(ChildNode.Position))) {
				// If the end node isn't walkable, we need to terminate the algorithm here.
				// If it isn't the end node, ignore it.
				continue;
			}

			ChildNode.ParentNodePosition = CurrentNode.Position;
			ChildNode.DistanceFromStartNode = CurrentNode.DistanceFromStartNode + 200;
			ChildNode.EstimatedDistanceToEndNode = (GetTotalDistanceBetweenTwoPositions(ChildNode.Position, EndPosition) * 200);
			ChildNode.TotalCost = ChildNode.DistanceFromStartNode + ChildNode.EstimatedDistanceToEndNode;
			ChildNode.IsValid = true;
			
			// Check if the child node is in the open node list.
			// if it isn't, add it.
			if (!OpenList.Contains(ChildNode)) {
				OpenList.Add(ChildNode);
			} else {
				// if the node is already in the open list, check that the path to this node is better?
				if (ChildNode.EstimatedDistanceToEndNode < CurrentNode.EstimatedDistanceToEndNode) {
					// if the child node is the better path, change the parent to the current node and recalculate the G and F values?
					ChildNode.ParentNodePosition = CurrentNode.Position;
				}
			}
		}
	}

	return Path;
}