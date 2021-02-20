#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Starmark_GameMode.h"

#include "Starmark_GameInstance.generated.h"


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

// Functions
// --------------------------------------------------

// ------------------------- Multi-player
	UFUNCTION(BlueprintImplementableEvent)
	void BeginHostingGame();
};
