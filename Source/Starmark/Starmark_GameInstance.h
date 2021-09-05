#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Starmark_Variables.h"

#include "Starmark_GameInstance.generated.h"

// Forward Declarations
class UPlayer_SaveData;


UCLASS()
class STARMARK_API UStarmark_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Player
	UPROPERTY()
	FPlayer_Data PlayerData;

// ------------------------- PlayerState Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPlayer_SaveData* PlayerProfileReference;
};
