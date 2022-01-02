#pragma once

#include "CoreMinimal.h"

#include "Character_Pathfinder.h"
#include "Components/StaticMeshComponent.h"

#include "Character_NonAvatarEntity.generated.h"


// Forward Declarations
class ACharacter_Pathfinder;


// Unique Enums
UENUM(BlueprintType)
enum class E_NonAvatarEntity_EntityType : uint8
{
	RockWall,
	Hurricane,
};


UCLASS()
class STARMARK_API ACharacter_NonAvatarEntity : public ACharacter_Pathfinder
{
	GENERATED_BODY()

	ACharacter_NonAvatarEntity();

public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CubeComponent;

// ------------------------- Entity
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	E_NonAvatarEntity_EntityType EntityType;

// Functions
// --------------------------------------------------

// ------------------------- Hurricane
	UFUNCTION()
	void HurricaneOnSpawn();

	UFUNCTION(BlueprintCallable)
	void HurricaneOnAvatarCollision(ACharacter_Pathfinder* Avatar);
};