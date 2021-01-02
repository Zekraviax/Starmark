#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Starmark_GameMode.generated.h"


// Enums
// --------------------------------------------------

//------------------------- User
UENUM(BlueprintType)
enum class E_User_Hemispheres : uint8
{
	E_Northern,
	E_Southern,
};

UENUM(BlueprintType)
enum class E_User_Pronouns : uint8
{
	E_Masculine,
	E_Feminine,
	E_Neutral
};

//------------------------- Character
UENUM(BlueprintType)
enum class E_Character_Types : uint8
{
	E_Fire,
	E_Water,
	E_Earth,
	E_Air,
};


// Structs
// --------------------------------------------------

//------------------------- 
USTRUCT(BlueprintType)
struct STARMARK_API F_User_Data
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	int Birthday_Day;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	int Birthday_Month;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	int Birthday_Year;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere")
	E_User_Hemispheres Hemisphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	E_User_Pronouns Pronouns;

	F_User_Data()
	{
		Birthday_Day = 1;
		Birthday_Month = 1;
		Birthday_Year = 2021;
		Hemisphere = E_User_Hemispheres::E_Southern;
		Name = "Default";
		Pronouns = E_User_Pronouns::E_Neutral;
	}
};


UCLASS()
class STARMARK_API AStarmark_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
};
