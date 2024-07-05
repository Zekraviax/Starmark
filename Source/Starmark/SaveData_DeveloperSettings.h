#pragma once


#include "CoreMinimal.h"

#include "GameFramework/SaveGame.h"
#include "Starmark_Variables.h"

#include "SaveData_DeveloperSettings.generated.h"


USTRUCT(BlueprintType)
struct STARMARK_API FDeveloperSettingsAsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RecalculateAvatarStatsAtStartOfBattle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PrintMessageOnGameStart;

	FDeveloperSettingsAsStruct()
	{
		RecalculateAvatarStatsAtStartOfBattle = true;
		PrintMessageOnGameStart = "test";
	}
};


UCLASS()
class STARMARK_API USaveData_DeveloperSettings : public USaveGame
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDeveloperSettingsAsStruct DevSettingsStruct;

// Functions
// --------------------------------------------------
	UFUNCTION(BlueprintCallable)
	static void SavePlayerDataToJson();

	UFUNCTION(BlueprintCallable)
	static void LoadPlayerDataFromJson();
};
