#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Materials/Material.h"
#include "Components/StaticMeshComponent.h"

#include "Actor_CharacterTest.generated.h"

UCLASS()
class STARMARK_API AActor_CharacterTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_CharacterTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// Base Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterTest")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterTest")
	UMaterialInterface* StaticMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterTest")
	UMaterialInstanceDynamic* DynamicMaterial;
};
