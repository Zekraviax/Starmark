#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerState_Base.generated.h"


// Forward Declarations
class ACharacter_Pathfinder;


UCLASS()
class STARMARK_API APlayerState_Base : public APlayerState
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- Battle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACharacter_Pathfinder* PlayerState_CurrentControlledAvatar;
};
