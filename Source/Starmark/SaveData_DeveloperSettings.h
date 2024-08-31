#pragma once


#include "CoreMinimal.h"

#include "GameFramework/SaveGame.h"

#include "SaveData_DeveloperSettings.generated.h"


USTRUCT(BlueprintType)
struct STARMARK_API FDeveloperSettingsAsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AutoLoadFirstFoundPlayerProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RecalculateAvatarStatsAtStartOfBattle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PrintMessageOnGameStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool EnableMultiplayerDumpToLogButton;

	FDeveloperSettingsAsStruct()
	{
		AutoLoadFirstFoundPlayerProfile = true;
		RecalculateAvatarStatsAtStartOfBattle = true;
		PrintMessageOnGameStart = "test";
		EnableMultiplayerDumpToLogButton = true;
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
	void SavePlayerDataToJson();

	UFUNCTION(BlueprintCallable)
	void LoadPlayerDataFromJson();

	UFUNCTION(BlueprintCallable)
	void DumpMultiplayerStateToJsonLog();
};
