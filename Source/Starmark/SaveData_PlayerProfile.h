#pragma once


#include "CoreMinimal.h"

#include "GameFramework/SaveGame.h"

#include "SaveData_PlayerProfile.generated.h"


USTRUCT(BlueprintType)
struct STARMARK_API FPlayerProfileAsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ProfileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;
};


UCLASS()
class STARMARK_API USaveData_PlayerProfile : public USaveGame
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	void SaveProfileDataToJson();

	UFUNCTION(BlueprintCallable)
	void LoadProfileDataFromJson();
};
