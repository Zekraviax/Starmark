#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "Starmark_Variables.h"

#include "Player_SaveData.generated.h"


UCLASS()
class STARMARK_API UPlayer_SaveData : public USaveGame
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Base
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY()
	FAvatar_Struct TeamSlotOne;

	UPROPERTY()
	FAvatar_Struct TeamSlotTwo;

	UPROPERTY()
	FAvatar_Struct TeamSlotThree;

	UPROPERTY()
	FAvatar_Struct TeamSlotFour;
};
