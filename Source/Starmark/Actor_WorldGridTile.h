#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Starmark_GameMode.h"

#include "Actor_WorldGridTile.generated.h"


USTRUCT(BlueprintType)
struct FGridTile
{
	GENERATED_USTRUCT_BODY()

	FGridTile() {};

	// Coordinates in Cube Space
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FHCubeCoord CubeCoord;

	// Coordinates
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		FVector WorldPosition
	{
		FVector::ZeroVector
	};

	// The Movement Cost of the tile
	// For a well-executed GraphAStar Pathfinder, the value must be at least 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1))
	float Cost{};

	// Is the tile a blocking tile
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsBlocking{};

	friend bool operator==(const FGridTile &A, const FGridTile &B)
	{
		return (A.CubeCoord == B.CubeCoord) && (A.Cost == B.Cost) && (A.bIsBlocking == B.bIsBlocking);
	}

	friend bool operator!=(const FGridTile &A, const FGridTile &B)
	{
		return !(A == B);
	}
};


UCLASS()
class STARMARK_API AActor_WorldGridTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_WorldGridTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
