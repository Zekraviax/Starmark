#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "Engine/DataTable.h"

#include "Starmark_GameMode.generated.h"

// Forward Declarations
class ACharacter_Pathfinder;


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


//------------------------- Avatar
UENUM(BlueprintType)
enum class EAvatar_Types : uint8
{
	// Base Elements
	E_Fire							UMETA(DisplayName = "Fire"),
	E_Water							UMETA(DisplayName = "Water"),
	E_Air							UMETA(DisplayName = "Air"),
	E_Earth							UMETA(DisplayName = "Earth"),
	E_Light							UMETA(DisplayName = "Light"),
	E_Dark							UMETA(DisplayName = "Dark"),
	E_Arcane						UMETA(DisplayName = "Arcane"),
	// Combination Elements
	E_Hex							UMETA(DisplayName = "Hex"),
	E_Fae							UMETA(DisplayName = "Fae"),
	E_Illusion						UMETA(DisplayName = "Illusion"),
	E_PSI							UMETA(DisplayName = "PSI"),
	E_Runic							UMETA(DisplayName = "Runic"),
	E_Spirit						UMETA(DisplayName = "Spirit"),
	E_Balance						UMETA(DisplayName = "Balance"),
	E_Toxin							UMETA(DisplayName = "Chem"),
	E_Diabolic						UMETA(DisplayName = "Diabolic"),
	E_Metal							UMETA(DisplayName = "Metal"),
	E_Cosmic						UMETA(DisplayName = "Cosmic"),
	E_Divine						UMETA(DisplayName = "Divine"),
	E_Nuclear						UMETA(DisplayName = "Nuclear"),
	E_Prism							UMETA(DisplayName = "Prism"),
	E_Cyber							UMETA(DisplayName = "Cyber"),
	E_Steam							UMETA(DisplayName = "Steam"),
	E_Nature						UMETA(DisplayName = "Nature"),
	E_Ice							UMETA(DisplayName = "Ice"),
	E_Magma							UMETA(DisplayName = "Magma"),
	E_Electric						UMETA(DisplayName = "Electric"),
	E_Sound							UMETA(DisplayName = "Sound"),
	// Placeholder
	E_None							UMETA(DisplayName = "N/A"),
};


UENUM(BlueprintType)
enum class EAvatar_Marks : uint8
{
	E_Romp							UMETA(DisplayName = "Romp"),
	E_Slith							UMETA(DisplayName = "Slith"),
	E_Roa							UMETA(DisplayName = "Roa"),
	E_Skip							UMETA(DisplayName = "Skip"),
	E_Flok							UMETA(DisplayName = "Flok"),
	E_Gup							UMETA(DisplayName = "Gup"),
	E_Xash							UMETA(DisplayName = "Xash"),
	E_Zwor							UMETA(DisplayName = "Zwor"),
	E_Kop							UMETA(DisplayName = "Kop"),
};


//------------------------- Grid
UENUM(BlueprintType)
enum class EHTileOrientationFlag : uint8
{
	FLAT,
	POINTY,
	NONE
};


//------------------------- Battle
UENUM(BlueprintType)
enum class EBattle_AttackPatterns : uint8
{
	Circle,
	FourWayCross,
	EightWayCross,
};


UENUM(BlueprintType)
enum class EBattle_AttackEffects : uint8
{
	// Status Effects
	AddParalyzeStatus,
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
	EAvatar_Marks Mark;

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
		Mark = EAvatar_Marks::E_Romp;
		Hemisphere = EPlayer_Hemispheres::E_Southern;
		Name = "Default";
		Pronouns = EPlayer_Pronouns::E_Neutral;
	}
};


//------------------------- Avatar
USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_SimpleTypeChart : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAvatar_Types Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAvatar_Types> TakesMoreDamageFromTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAvatar_Types> TakesLessDamageFromTypes;

	FAvatar_SimpleTypeChart()
	{
		Type = EAvatar_Types::E_Fire;
		TakesMoreDamageFromTypes.Add(EAvatar_Types::E_Water);
		TakesLessDamageFromTypes.Add(EAvatar_Types::E_Air);
		//TakesMoreDamageFromType = EAvatar_Types::E_Water;
		//TakesLessDamageFromType = EAvatar_Types::E_Air;
	}
};

USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_UltimateTypeChart : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAvatar_Types Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAvatar_Types> CombinationTypes;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TArray<EAvatar_Types> TakesMoreDamageFromTypes;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TArray<EAvatar_Types> TakesLessDamageFromTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAvatar_Types> DoesMoreDamageToTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAvatar_Types> DoesLessDamageToTypes;

	FAvatar_UltimateTypeChart()
	{
		Type = EAvatar_Types::E_Hex;
		CombinationTypes.Add(EAvatar_Types::E_Arcane);
		CombinationTypes.Add(EAvatar_Types::E_Dark);
		//TakesMoreDamageFromTypes.Add(EAvatar_Types::E_Earth);
		//TakesMoreDamageFromTypes.Add(EAvatar_Types::E_Arcane);
		//TakesLessDamageFromTypes.Add(EAvatar_Types::E_Light);
		//TakesLessDamageFromTypes.Add(EAvatar_Types::E_Dark);
		//DoesMoreDamageToTypes.Add(EAvatar_Types::E_Earth);
		//DoesMoreDamageToTypes.Add(EAvatar_Types::E_Dark);
		//DoesLessDamageToTypes.Add(EAvatar_Types::E_Light);
		//DoesLessDamageToTypes.Add(EAvatar_Types::E_Air);
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_CombinationTypes : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAvatar_Types PrimaryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAvatar_Types SecondaryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAvatar_Types CombinationType;

	FAvatar_CombinationTypes()
	{
		PrimaryType = EAvatar_Types::E_Fire;
		SecondaryType = EAvatar_Types::E_Water;
		CombinationType = EAvatar_Types::E_Steam;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_DyeColours : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Colour;

	FAvatar_DyeColours()
	{

	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_BaseStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HealthPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ManaPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Defence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Evade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Power;

	FAvatar_BaseStats()
	{
		HealthPoints = 10;
		ManaPoints = 10;
		Attack = 1;
		Defence = 1;
		Speed = 1;
		Evade = 1;
		Power = 1;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_Size
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SizeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SizeY;

	FAvatar_Size()
	{
		SizeX = 1;
		SizeY = 1;
	}

	FAvatar_Size(int InSizeX, int InSizeY)
	{
		SizeX = InSizeX;
		SizeY = InSizeY;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_ElementalEssences
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FireEssence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WaterEssence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AirEssence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EarthEssence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LightEssence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DarkEssence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ArcaneEssence;

	FAvatar_ElementalEssences()
	{
		FireEssence = 0;
		WaterEssence = 0;
		AirEssence = 0;
		EarthEssence = 0;
		LightEssence = 0;
		DarkEssence = 0;
		ArcaneEssence = 0;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_StatusEffect : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TurnsRemaining;

	FAvatar_StatusEffect() 
	{
		Name = "Default";
		Image = nullptr;
		TurnsRemaining = 1;
	}

	FAvatar_StatusEffect(FString InName, UTexture2D* InImage, int InTurnsRemaining)
	{
		Name = InName;
		Image = InImage;
		TurnsRemaining = InTurnsRemaining;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_AttackStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAvatar_Types Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BasePower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BaseRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ManaCost;

	// Attack Pattern
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBattle_AttackPatterns AttackPattern;

	// Effect: On attack launch
	// Effect: On Self
	// Effect: On Target(s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EBattle_AttackEffects> AttackEffectsOnTarget;

	FAvatar_AttackStruct()
	{
		Name = "Default";
		Type = EAvatar_Types::E_Air;
		BasePower = 1;
		BaseRange = 1;
		ManaCost = 1;
		AttackPattern = EBattle_AttackPatterns::Circle;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_Struct : public FTableRowBase
{
	GENERATED_BODY()

// ------------------------- Base Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	FString AvatarName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	FString Nickname;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	EAvatar_Types PrimaryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	EAvatar_Types SecondaryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	EAvatar_Marks Mark;

// ------------------------- Battle
	// Instinct Abilities

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FAvatar_BaseStats BaseStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int CurrentHealthPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int CurrentManaPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int MaximumTileMoves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int CurrentTileMoves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int MaximumMovePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int CurrentMovePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int MaximumActionPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int CurrentActionPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float SameTypeAttackBonusMultiplier;


// ------------------------- Appearance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FAvatar_Size Size;

	// 3D Model
	// Material
	// Default Colour
	// Animations
	// Menu Image(s)

// ------------------------- Other Data Tables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other DataTables")
	TArray<FDataTableRowHandle> SimpleAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other DataTables")
	TArray<FDataTableRowHandle> AttacksLearnedByBuyingWithEssence;

// ------------------------- Encyclopedia
	// Number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encyclopedia")
	int EncyclopediaNumber;

	// Height Range
	// Weight Range

	// Lore
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encyclopedia")
	FString Lore;


// ------------------------- Other Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	TArray<FIntPoint> OccupiedTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	int Tier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	int TokensRequired;


	FAvatar_Struct()
	{
		AvatarName = "Default";
		Nickname = "";
		Mark = EAvatar_Marks::E_Flok;
		PrimaryType = EAvatar_Types::E_Air;
		SecondaryType = EAvatar_Types::E_None;
		CurrentHealthPoints = 1;
		CurrentManaPoints = 1;
		MaximumTileMoves = 2;
		CurrentTileMoves = 2;
		MaximumMovePoints = 1;
		CurrentMovePoints = 1;
		MaximumActionPoints = 1;
		CurrentActionPoints = 1;
		SameTypeAttackBonusMultiplier = 150;
		EncyclopediaNumber = 0;
		Lore = "Default";
		OccupiedTiles.AddUnique(FIntPoint(0, 0));
		Tier = 1;
		TokensRequired = 1;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint QR
	{
		FIntPoint::ZeroValue
	};

	FHAxialCoord() {}

	FHAxialCoord(int32 q, int32 r)
	{
		QR.X = q;
		QR.Y = r;
	}

	FHAxialCoord(FIntPoint _qr) : QR(_qr)
	{

	}
};

USTRUCT(BlueprintType)
struct FHCubeCoord
{
	GENERATED_USTRUCT_BODY()

	FHCubeCoord() {}

	FHCubeCoord(int32 q, int32 r, int32 s)
	{
		//check(q + r + s == 0);
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
			lhs.QRS + rhs.QRS
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector QRS
	{
		FVector::ZeroVector
	};

	FHFractional() {}

	FHFractional(float q, float r, float s)
	{
		QRS.X = q;
		QRS.Y = r;
		QRS.Z = q;
	}

	FHFractional(FVector _v) : QRS(_v)
	{

	}
};

USTRUCT(BlueprintType)
struct FHDirections
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FHCubeCoord> Directions;

	FHDirections() {}
};

USTRUCT(BlueprintType)
struct FHDiagonals
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FHCubeCoord> Diagonals;

	FHDiagonals() {}
};

USTRUCT(BlueprintType)
struct FHTileOrientation
{
	GENERATED_USTRUCT_BODY()

	// F variables are used in the GridToWorld conversion
	// f0 and f1 for X coords, f2 and f3 for Y coords
	double f0, f1, f2, f3;

	// B variables are used in the WorldToGrid conversion
	double b0, b1, b2, b3;

	FHTileOrientation() 
	{

	}

	friend bool operator==(const FHTileOrientation &lhs, const FHTileOrientation &rhs) 
	{
		return (lhs.f0 == rhs.f0) && (lhs.f1 == rhs.f1) && (lhs.f2 == rhs.f2) && (lhs.f3 == rhs.f3);
	}


};

const struct FHFlatTopOrientation : FHTileOrientation
{
	FHFlatTopOrientation()
	{
		// UE4 | Original
		f0 = -FMath::Sqrt(3.0) / 2.0;	// -f2 | f0 = 3/2
		f1 = -FMath::Sqrt(3.0);			// -f3 | f1 = 0
		f2 = 3.0 / 2.0;					//  f0 | f2 = sqrt(3)/2
		f3 = 0.0;						//  f1 | f3 = sqrt(3)

		b0 = 0.0;						// b1 | b0 = 2/3
		b1 = 2.0 / 3.0;					// b0 | b1 = 0
		b2 = FMath::Sqrt(3.0) / 3.0;	// b3 | b2 = -1/3
		b3 = -1.0 / 3.0;				// b2 | b3 = sqrt(3)/3
	}

} HFlatTopLayout;

const struct FHPointyOrientation : FHTileOrientation
{
	FHPointyOrientation()
	{
		f0 = 0.0;						// -f2 | f0 = sqrt(3)
		f1 = -3.0 / 2.0;				// -f3 | f1 = sqrt(3)/2
		f2 = FMath::Sqrt(3.0);			//  f0 | f2 = 0
		f3 = FMath::Sqrt(3.0) / 2.0;	//  f1 | f3 = 3/2f

		b0 = 1.0 / 3.0;					// -b1 | b0 = sqrt(3)/3
		b1 = FMath::Sqrt(3.0) / 3.0;	//  b0 | b1 = -1/3
		b2 = -2.0 / 3.0;				// -b3 | b2 = 0
		b3 = 0.0;						//  b2 | b3 = 2/3
	}

} HPointyOrientation;


UCLASS()
class STARMARK_API AStarmark_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void GameMode_LaunchAttack(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Battle_SubtractHealth(ACharacter_Pathfinder* Defender, int DamageDealt);

	UFUNCTION()
	void Battle_AvatarDefeated(ACharacter_Pathfinder* Avatar);

	UFUNCTION(BlueprintNativeEvent)
	void EndOfBattle();
};