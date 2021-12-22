#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Starmark_Variables.h"

#include "Actor_GridTile.generated.h"


// Unique Enums
UENUM(BlueprintType)
enum class E_GridTile_ColourChangeContext : uint8
{
	Normal,
	OnMouseHover,
	OnMouseHoverTileUnreachable,
	WithinAttackRange,
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Floor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* GridTileHitbox;

// ------------------------- Tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<E_GridTile_Properties> Properties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* DynamicMaterial;

	// Occupying Actor
	UPROPERTY()
	AActor* OccupyingActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MovementCost = 1;

// ------------------------- Multiplayer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AssignedMultiplayerUniqueID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AvatarSlotNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ChangeColourOnMouseHover = true;

// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable)
	void UpdateGridTileState();

	UFUNCTION(BlueprintCallable)
	void OnMouseBeginHover();

	UFUNCTION(BlueprintCallable)
	void OnMouseEndHover();

	UFUNCTION(BlueprintCallable)
	void UpdateTileColour(E_GridTile_ColourChangeContext ColourChangeContext);
};
