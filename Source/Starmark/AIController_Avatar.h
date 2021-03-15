#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIController_Avatar.generated.h"


UCLASS()
class STARMARK_API AAIController_Avatar : public AAIController
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// -------------------------

// Functions
// --------------------------------------------------

// ------------------------- AI
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void GetMoveCommandFromPlayer(FVector MoveLocation);
};
