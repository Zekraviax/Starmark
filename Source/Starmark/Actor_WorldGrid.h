#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor_WorldGrid.generated.h"


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
	UFUNCTION()
	bool IsValidGridCell(const FIntPoint& Location) const;

	UFUNCTION()
	bool IsGridCellWalkable(const FIntPoint& Location) const;

	UFUNCTION()
	bool GetGridCellForWorldPos(const FVector& WorldPos, FIntPoint& GridPos) const;
};