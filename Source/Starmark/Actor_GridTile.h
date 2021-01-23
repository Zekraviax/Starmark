#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/StaticMeshComponent.h"

#include "Actor_GridTile.generated.h"


// Unique Enums
UENUM(BlueprintType)
enum class E_GridTile_TraversalProperties : uint8
{
	E_None,
	E_Wall,
	E_Occupied,
};


UCLASS()
class STARMARK_API AActor_GridTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_GridTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* Floor;

// ------------------------- Grid Tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Tile")
	TArray<E_GridTile_TraversalProperties> TraversalProperties;
};
