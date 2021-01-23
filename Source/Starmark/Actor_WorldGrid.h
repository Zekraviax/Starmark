#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor_WorldGrid.generated.h"


UCLASS()
class STARMARK_API AActor_WorldGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_WorldGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
