#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Engine/DataTable.h"

#include "Starmark_GameMode.generated.h"


// Enums
// --------------------------------------------------

//------------------------- Player
UENUM(BlueprintType)
enum class EPlayer_Hemispheres : uint8
{
	E_Northern,
	E_Southern,
};

UENUM(BlueprintType)
enum class EPlayer_Pronouns : uint8
{
	E_Masculine,
	E_Feminine,
	E_Neutral
};

UENUM(BlueprintType)
enum class EPlayer_Horoscopes : uint8
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
enum class ECharacter_Types : uint8
{
	// Base Elements
	E_Fire,
	E_Water,
	E_Earth,
	E_Air,
	E_Light,
	E_Dark,
	E_Arcane,
	// Combination Elements
	E_Hex,
	E_Fae,
	E_Illusion,
	E_PSI,
	E_Runic,
	E_Spirit,
	E_Balance,
	E_Toxin,
	E_Diabolic,
	E_Metal,
	E_Cosmic,
	E_Divine,
	E_Nuclear,
	E_Prism,
	E_Cyber,
	E_Steam,
	E_Nature,
	E_Ice,
	E_Magma,
	E_Electric,
	E_Sound,
};

UENUM(BlueprintType)
enum class ECharacter_Marks : uint8
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

//------------------------- Grid
UENUM(BlueprintType)
enum class EHTileOrientationFlag : uint8
{
	FLAT,
	POINTY,
	NONE
};


// Structs
// --------------------------------------------------

//------------------------- Player
USTRUCT(BlueprintType)
struct STARMARK_API FPlayer_Data
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	int Birthday_Day;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	int Birthday_Month;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	int Birthday_Year;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	EPlayer_Horoscopes Horoscope;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Birthday")
	ECharacter_Marks Mark;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere")
	EPlayer_Hemispheres Hemisphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	EPlayer_Pronouns Pronouns;

	FPlayer_Data()
	{
		Birthday_Day = 1;
		Birthday_Month = 1;
		Birthday_Year = 2021;
		Horoscope = EPlayer_Horoscopes::E_Capricorn;
		Mark = ECharacter_Marks::E_Romp;
		Hemisphere = EPlayer_Hemispheres::E_Southern;
		Name = "Default";
		Pronouns = EPlayer_Pronouns::E_Neutral;
	}
};


//------------------------- Character
USTRUCT(BlueprintType)
struct STARMARK_API FCharacter_TypeChart : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacter_Types Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacter_Types TakesMoreDamageFromType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacter_Types TakesLessDamageFromType;

	FCharacter_TypeChart()
	{
		Type = ECharacter_Types::E_Fire;
		TakesMoreDamageFromType = ECharacter_Types::E_Water;
		TakesLessDamageFromType = ECharacter_Types::E_Air;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FCharacter_CombinationTypes : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacter_Types PrimaryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacter_Types SecondaryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacter_Types CombinationType;

	FCharacter_CombinationTypes()
	{
		PrimaryType = ECharacter_Types::E_Fire;
		SecondaryType = ECharacter_Types::E_Water;
		CombinationType = ECharacter_Types::E_Steam;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FCharacter_DyeColours : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Colour;

	FCharacter_DyeColours()
	{

	}
};


//------------------------- Grid
USTRUCT(BlueprintType)
struct FHTileLayout
{
	GENERATED_USTRUCT_BODY()

	FHTileLayout() {}

	FHTileLayout(EHTileOrientationFlag orientation, float size, FVector origin) : TileOrientation(orientation), TileSize(size), Origin(origin) {
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHTileOrientationFlag TileOrientation
	{
		EHTileOrientationFlag::NONE
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TileSize
	{
		0
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Origin {
		FVector::ZeroVector
	};
};

USTRUCT(BLueprintType)
struct FHAxialCoord 
{
	GENERATED_USTRUCT_BODY()

	FHAxialCoord() {}
};

USTRUCT(BlueprintType)
struct FHCubeCoord
{
	GENERATED_USTRUCT_BODY()

	FHCubeCoord() {}

	FHCubeCoord(int32 q, int32 r, int32 s)
	{
		check(q + r + s == 0);
		QRS.X = q;
		QRS.Y = r;
		QRS.Z = s;
	}

	FHCubeCoord(FIntVector _v) : QRS(_v) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector QRS 
	{
		FIntVector::ZeroValue
	};

	friend FHCubeCoord operator+ (const FHCubeCoord &lhs, const FHCubeCoord &rhs)
	{
		return FHCubeCoord 
		{
			lhs.QRS + rhs.QRS;
		};
	}

	friend FHCubeCoord operator-(const FHCubeCoord &lhs, const FHCubeCoord &rhs)
	{
		return FHCubeCoord
		{ 
			lhs.QRS - rhs.QRS 
		};
	}

	friend FHCubeCoord operator*(const FHCubeCoord &lhs, int32 k)
	{
		return FHCubeCoord
		{ 
			lhs.QRS * k 
		};
	}

	friend bool operator==(const FHCubeCoord &lhs, const FHCubeCoord &rhs)
	{
		return lhs.QRS == rhs.QRS;
	}

	friend bool operator!=(const FHCubeCoord &lhs, const FHCubeCoord &rhs)
	{
		return lhs.QRS != rhs.QRS;
	}

	friend uint32 GetTypeHash(const FHCubeCoord &Other)
	{
		FString TypeHash
		{
			Other.QRS.ToString()
		};

		return GetTypeHash(TypeHash);
	}
};

USTRUCT(BlueprintType)
struct FHFractional
{
	GENERATED_USTRUCT_BODY()

	FHFractional() {}
};


UCLASS()
class STARMARK_API AStarmark_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
};
