#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "SaveData_PlayerProfilesList.generated.h"


// This class is deprecated. Please don't use it.
// Use the save file PlayerProfileHandler to load player JSON data.
UCLASS()
class STARMARK_API USaveData_PlayerProfilesList : public USaveGame
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- Save Data
	UPROPERTY()
	TArray<FString> PlayerProfileNames;

	UPROPERTY()
	TArray<FString> LastLoadedProfileName;
};
