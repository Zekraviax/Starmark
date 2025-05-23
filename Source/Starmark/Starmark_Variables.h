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
enum class EEntity_Factions : uint8 
{
	Player1,
	Enemy1,
};


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
	Special,		// The pattern is based on the attack itself, not the pattern
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
	AddMarkedStatus,
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
	int MaximumHealthPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaximumManaPoints;

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
		MaximumHealthPoints = 10;
		MaximumManaPoints = 10;
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

	FAvatar_ItemStruct()
	{
		Name = "Default";
		Description = "Default";
	}
};


USTRUCT(BlueprintType)
struct STARMARK_API FAvatar_Struct : public FFastArraySerializerItem
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

	// Currently used for testing combat only.
	// May or may not be deleted in the future.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FAvatar_BaseStats SpeciesMinimumStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FAvatar_BaseStats BattleStats;

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

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	//FAvatar_AbilityStruct Ability;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	//FAvatar_ItemStruct HeldItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TArray<EEntity_Factions> Factions;

	// The Owner Unique ID should be unique between players.
	// The Battle Unique ID should be unique between *all* avatars.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int OwnerMultiplayerUniqueID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int BattleUniqueID;

// ------------------------- Appearance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	UMaterial* DefaultImage;

	// Animations
	// Menu Image(s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TArray<UTexture2D*> UiImages;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	//FAvatar_Size Size;

	// 3D Model
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	//UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	UMaterial* DyableMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TArray<FLinearColor> DefaultColours;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	TArray<FLinearColor> CurrentColours;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	int DyableColourCount;

// ------------------------- Other Data Tables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other DataTables")
	TArray<FDataTableRowHandle> Attacks;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other DataTables")
	//TArray<FDataTableRowHandle> AttacksLearnedByBuyingWithEssence;

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

	// 0 is the default for newly created avatars
	// 1 - 4 are reserved for active avatars
	// 5 and 6 are reserved for reserve avatars
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
		CurrentHealthPoints = -1;
		CurrentManaPoints = -1;
		MaximumTileMoves = -1;
		CurrentTileMoves = -1;
		MaximumActionPoints = -1;
		CurrentActionPoints = -1;
		SameTypeAttackBonusMultiplier = 150;
		//Factions.AddUnique(EEntity_Factions::Player1);
		OwnerMultiplayerUniqueID = -1;
		BattleUniqueID = -1;
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


USTRUCT()
struct FWrappedAvatarData : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FAvatar_Struct> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FAvatar_Struct, FWrappedAvatarData>(Items, DeltaParms, *this);
	}
};


template<>
struct TStructOpsTypeTraits< FWrappedAvatarData > : public TStructOpsTypeTraitsBase2< FWrappedAvatarData >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};


//------------------------- Player
USTRUCT(BlueprintType)
struct STARMARK_API FPlayer_Data : public FFastArraySerializerItem
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
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	FString ProfileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
	EPlayer_Pronouns Pronouns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TArray<FAvatar_Struct> CurrentAvatarTeam;

	// -------- Multiplayer only -------- //
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int MultiplayerUniqueID;

	

	FPlayer_Data()
	{
		Birthday_Day = 1;
		Birthday_Month = 1;
		Birthday_Year = 2021;
		Horoscope = EPlayer_Horoscopes::E_Capricorn;
		Mark = EAvatar_Marks::E_Romp;
		Hemisphere = EPlayer_Hemispheres::E_Southern;
		PlayerName = "None";
		ProfileName = "None";
		Pronouns = EPlayer_Pronouns::E_Neutral;
		MultiplayerUniqueID = -1;
	}
};


USTRUCT()
struct FWrappedPlayerData : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FPlayer_Data> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPlayer_Data, FWrappedPlayerData>(Items, DeltaParms, *this);
	}
};


template<>
struct TStructOpsTypeTraits< FWrappedPlayerData > : public TStructOpsTypeTraitsBase2< FWrappedPlayerData >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};



class STARMARK_API Starmark_Variables
{
// To-Do: Put global variables here, like the save directory file path.
public:
// ------------------------- Global Helper Variables
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString PlayerDataSaveFilePath = "C:\\Users\\zekra\\Documents\\UE\\Projects\\Starmark\\Saved\\SaveGames";

	// If we put DataTable references here, then an instance of this class will need to be created when the game launches.
	// Same goes for widget blueprint class references.


// ------------------------- Global Helper Functions
	// To-Do: Make this function static?
	FString GetPlayerDataSaveFilePath() { return PlayerDataSaveFilePath; }
};