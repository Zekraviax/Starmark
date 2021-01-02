#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Starmark_GameMode.generated.h"


// Enums
// --------------------------------------------------

//------------------------- Player
UENUM(BlueprintType)
enum class E_Player_Hemispheres : uint8
{
	E_Northern,
	E_Southern,
};

UENUM(BlueprintType)
enum class E_Player_Pronouns : uint8
{
	E_Masculine,
	E_Feminine,
	E_Neutral
};

UENUM(BlueprintType)
enum class E_Player_Horoscopes : uint8
{
	E_Capricorn,
	E_Aquarius,
	E_Pisces,
	E_Aries,
	E_Taurus,
	E_Gemini,
	E_Cancer,
	E_Leo,
	E_Virgo,
	E_Libra,
	E_Scorpio,
	E_Sagittarius,
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

UENUM(BlueprintType)
enum class E_Character_Marks : uint8
{
	E_Romp,
	E_Slith,
	E_Roa,
	E_Skip,
	E_Flok,
	E_Gup,
	E_Xash,
	E_Zwor,
	E_Kop,
};


// Structs
// --------------------------------------------------

//------------------------- 
USTRUCT(BlueprintType)
struct STARMARK_API F_Player_Data
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	int Birthday_Day;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	int Birthday_Month;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	int Birthday_Year;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	E_Player_Horoscopes Horoscope;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	E_Character_Marks Mark;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere")
	E_Player_Hemispheres Hemisphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	E_Player_Pronouns Pronouns;

	F_Player_Data()
	{
		Birthday_Day = 1;
		Birthday_Month = 1;
		Birthday_Year = 2021;
		Horoscope = E_Player_Horoscopes::E_Capricorn;
		Mark = E_Character_Marks::E_Romp;
		Hemisphere = E_Player_Hemispheres::E_Southern;
		Name = "Default";
		Pronouns = E_Player_Pronouns::E_Neutral;
	}
};


UCLASS()
class STARMARK_API AStarmark_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
};
