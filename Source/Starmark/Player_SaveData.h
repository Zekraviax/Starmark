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
	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString ProfileName;

// ------------------------- Avatars
	UPROPERTY()
	TArray<FAvatar_Struct> AvatarLibrary;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FAvatar_Struct> CurrentAvatarTeam;
};
