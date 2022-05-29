#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor_HatTrick.generated.h"


// Forward Declarations
class AActor_GridTile;


UCLASS()
class STARMARK_API AActor_HatTrick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_HatTrick();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
// Variables
// --------------------------------------------------

// -------------------------
	UPROPERTY()
	TArray<AActor_GridTile*> HatTilesArray;
	
// Functions
// --------------------------------------------------

// -------------------------
	UFUNCTION()
	void SelectHatToHideAvatar();
};