#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Starmark_GameMode.h"

#include "Starmark_GameInstance.generated.h"

// Forward Declarations
//class AStarmark_GameMode;


UCLASS()
class STARMARK_API UStarmark_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
// Base Variables
// --------------------------------------------------

// ------------------------- Player
	UPROPERTY()
	F_Player_Data PlayerData;
};
