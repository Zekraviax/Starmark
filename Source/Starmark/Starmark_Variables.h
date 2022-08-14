#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"

#include "Starmark_Variables.generated.h"


// Forward Declarations
class AActor_AbilitiesLibrary;
class AActor_StatusEffectsLibrary;
class ACharacter_Pathfinder;
class APlayerPawn_Static;


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
enum class E_GridTile_Properties : uint8
{
	E_None						UMETA(DisplayName = "None"),
	E_Wall,
	E_Occupied,
	E_PlayerAvatarSpawn,
	E_StoneRoad,
	Shadow,
	Fire,
	Hat,
	Haze,
};


//------------------------- Battle
UENUM(BlueprintType)
enum class EBattle_AttackPatterns : uint8
{
	Circle,
	Cone,
	FourWayCross,
	EightWayCross,
	AOE_Circle,
	SingleTile,
	Ring,
	// Special patterns for single attacks
	WideWall,
	TwoTiles,
};


UENUM(BlueprintType)
enum class EBattle_AttackCategories : uint8
{
	Offensive,
	Utility,
	Environment,
};


UENUM(BlueprintType)
enum class EBattle_AttackTargetsInRange : uint8
{
	AttackAllTargets,
	AttackClickedAvatar,
	SelectAllGridTiles,
	SelectAllGridTilesAndSelectAllAvatars,
	Self,
	SelectTwoTilesAndSelf,
};


UENUM(BlueprintType)
enum class EBattle_AttackEffects : uint8
{
	// Status Effects
	AddParalyzeStatus,
	AddBurnStatus,
	AddDrowningStatus,
	AddStoneSkinStatus,
	AddBleedStatus,
	AddSoakStatus,
	AddSpellboundStatus,
	AddVampirismStatus,
	// Other
	KnockbackTarget,
	NoFriendlyFire,
	RefundManaPointsPerTargetHit,
	TransferManaPoints,
	// Change Grid Tile Properties
	SpawnWall,
	SpawnHurricane,
	SpawnHats,
	AddPropertyStoneRoad,
	AddPropertyShadow,
	AddPropertyFire,
	// Alternative Attack Functions/Properties
	LowerTargetHealthEqualsHigherDamageDealt,
	// Special Functions
	SummonAvatar,
	// Full Move Functions
	Light_Illuminate,
};


UENUM(BlueprintType)
enum class E_Ability_Functions : uint8
{
	Regenerate,
};


UENUM(BlueprintType)
enum class E_Ability_TriggerConditions : uint8
{
	Passive,
	OnAvatarDefeated,
	OnAvatarStartOfTurn,
};


// UI
UENUM(BlueprintType)
enum class E_RightClickMenu_Commands : uint8
{
	Cancel,
	EditAvatar,
	EquipAvatar,
	UnequipAvatar,
	DeleteAvatar,
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAvatar_Types> DoesMoreDamageToTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAvatar_Types> DoesLessDamageToTypes;

	FAvatar_UltimateTypeChart()
	{
		Type = EAvatar_Types::E_Hex;
		CombinationTypes.Add(EAvatar_Types::E_Arcane);
		CombinationTypes.Add(EAvatar_Types::E_Dark);
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
	int SpecialAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpecialDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Evade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Power;

	FAvatar_BaseStats()
	{
		HealthPoints = 100;
		ManaPoints = 100;
		Attack = 10;
		Defence = 10;
		SpecialAttack = 10;
		SpecialDefence = 10;
		Speed = 10;
		Evade = 10;
		Power = 10;
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
	FString Description;

	// How long the status effect lasts
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaximumTurns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TurnsRemaining;

	// Use the StatusEffectsLibrary to handle special functions such as remembering variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor_StatusEffectsLibrary* SpecialFunctionsActor;

	FAvatar_StatusEffect()
	{
		Name = "Default";
		Image = nullptr;
		Description = "Default";
		MaximumTurns = 1;
		TurnsRemaining = 1;
		SpecialFunctionsActor = nullptr;
	}

	FAvatar_StatusEffect(FString InName, UTexture2D* InImage, FString InDescription, int InMaxinumTurns, int InTurnsRemaining)
	{
		Name = InName;
		Image = InImage;
		Description = InDescription;
		MaximumTurns = InMaxinumTurns;
		TurnsRemaining = InTurnsRemaining;
		//SpecialFunctionsActor = InSpecialFunctionsActor;
	}

	bool operator==(const FAvatar_StatusEffect& OtherStatusEffect) const
	{
		return (Name == OtherStatusEffect.Name &&
				Image == OtherStatusEffect.Image &&
				Description == OtherStatusEffect.Description &&
				SpecialFunctionsActor == OtherStatusEffect.SpecialFunctionsActor);
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
	
	// Attack Pattern
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBattle_AttackPatterns AttackPattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBattle_AttackCategories AttackCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBattle_AttackTargetsInRange AttackTargetsInRange;

	// Effect: On attack launch
	// Effect: On Self
	// Effect: On Target(s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EBattle_AttackEffects> AttackEffectsOnTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AttachAttackTraceActorToMouse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RotateAvatarTowardsMouse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool EndAvatarTurnOnUse;

	FAvatar_AttackStruct()
	{
		Name = "Default";
		Type = EAvatar_Types::E_Air;
		BasePower = 1;
		BaseRange = 1;
		ManaCost = 1;
		Description = "Default.";
		AttackPattern = EBattle_AttackPatterns::Circle;
		//AttackCategory =
		//AttackTargetsInRange = 
		AttachAttackTraceActorToMouse = false;
		RotateAvatarTowardsMouse = true;
		EndAvatarTurnOnUse = true;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_AbilityStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	E_Ability_Functions Function;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	E_Ability_TriggerConditions TriggerCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor_AbilitiesLibrary* AbilityLibraryActor;
	
	FAvatar_AbilityStruct()
	{
		Name = "Default";
		Function = E_Ability_Functions::Regenerate;
		TriggerCondition = E_Ability_TriggerConditions::Passive;
		Description = "Default";
		AbilityLibraryActor = nullptr;
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_ItemStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	//TArray<FAvatar_ItemFunctions> ItemFunctions

	FAvatar_ItemStruct()
	{
		Name = "Default";
		Description = "Default";
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
	int MaximumActionPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int CurrentActionPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	float SameTypeAttackBonusMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TArray<FAvatar_AttackStruct> CurrentAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FAvatar_AbilityStruct Ability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FAvatar_ItemStruct HeldItem;

// ------------------------- Appearance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	UMaterial* DefaultImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FAvatar_Size Size;

	// 3D Model
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	//UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	USkeletalMesh* SkeletalMesh;

	// Dyable Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	UMaterial* DyableMaterial;

	// Colours
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TArray<FLinearColor> DefaultColours;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TArray<FLinearColor> CurrentColours;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	int DyableColourCount;

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

	// 0 - 3 are reserved for active avatars
	// 4 and 5 are reserved for reserve avatars
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	int IndexInPlayerLibrary;

	// Fetch all the attacks from the datatable when the battle starts,
	// not when the avatar is created or edited
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	TArray<FName> CurrentEquippedAttackNames;

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
		MaximumActionPoints = 1;
		CurrentActionPoints = 1;
		SameTypeAttackBonusMultiplier = 150;
		DefaultImage = nullptr;
		SkeletalMesh = nullptr;
		DyableMaterial = nullptr;
		DyableColourCount = 0;
		EncyclopediaNumber = 0;
		Lore = "Default";
		OccupiedTiles.AddUnique(FIntPoint(0, 0));
		Tier = 1;
		TokensRequired = 1;
		IndexInPlayerLibrary = -1000000;
	}

	bool operator==(const FAvatar_Struct& OtherAvatar) const
	{
		return (AvatarName == OtherAvatar.AvatarName &&
			Nickname == OtherAvatar.Nickname);
	}
};


class STARMARK_API Starmark_Variables
{

};