#pragma once


#include "CoreMinimal.h"

#include "GameFramework/SaveGame.h"
#include "Starmark_Variables.h"

#include "SaveData_PlayerProfile.generated.h"


USTRUCT(BlueprintType)
struct STARMARK_API FPlayerProfileAsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ProfileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAvatar_Struct> Team;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAvatar_Struct> Library;
};


UCLASS()
class STARMARK_API USaveData_PlayerProfile : public USaveGame
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------
	// To-Do: Put this variable somewhere global, so it can easily be accessed and edited
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString PlayerDataSaveFilePath = "C:\\Users\\zekra\\Documents\\UE\\Projects\\Starmark\\Saved\\SaveGames";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerProfileAsStruct PlayerProfileStruct;
	
// Functions
// --------------------------------------------------
	UFUNCTION(BlueprintCallable)
	void SaveProfileDataToJson();

	UFUNCTION(BlueprintCallable)
	void LoadProfileDataFromJson(FString InProfileName);
};
