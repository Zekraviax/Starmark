#pragma once


#include "CoreMinimal.h"

#include "GameFramework/SaveGame.h"

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
	// To-Do: Put this variable somewhere global, so it can easily be accessed and edited
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerDataSaveFilePath = "C:\\Users\\zekra\\Documents\\UE\\Projects\\Starmark\\Saved\\SaveGames";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDeveloperSettingsAsStruct DevSettingsStruct;

// Functions
// --------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void SavePlayerDataToJson();

	UFUNCTION(BlueprintCallable)
	void LoadPlayerDataFromJson();
};
