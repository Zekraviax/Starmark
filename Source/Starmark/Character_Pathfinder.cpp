#include "Character_Pathfinder.h"

#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "PlayerController_Base.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "Starmark_GameState.h"
#include "Starmark_GameMode.h"


ACharacter_Pathfinder::ACharacter_Pathfinder()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 69.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//// Create a camera boom...
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	//CameraBoom->TargetArmLength = 800.f;
	//CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	//CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	//// Create a camera...
	//TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	//TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Cursor To World Decal
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	//static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Materials/M_Cursor_Decal.M_Cursor_Decal'"));
	//if (DecalMaterialAsset.Succeeded())
	//{
	//	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	//}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	CursorToWorld->SetVisibility(false);

	// Actor Selected Decal
	ActorSelected = CreateDefaultSubobject<UDecalComponent>("ActorSelected");
	ActorSelected->SetupAttachment(RootComponent);
	//static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Materials/M_Cursor_Decal.M_Cursor_Decal'"));
	//if (DecalMaterialAsset.Succeeded())
	//{
	//	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	//}
	ActorSelected->DecalSize = FVector(32.0f, 64.0f, 64.0f);
	ActorSelected->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	ActorSelected->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	ActorSelected->SetVisibility(false);

	// AvatarBattleData WidgetComponent
	AvatarBattleData_Component = CreateDefaultSubobject<UWidgetComponent>("AvatarBattleData_Component");
	AvatarBattleData_Component->SetupAttachment(RootComponent);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Battle Testing
	CurrentLevel = 1;
	//CurrentHealthPoints = AvatarData.BaseStats.HealthPoints;
	//CurrentManaPoints = AvatarData.BaseStats.ManaPoints;
	//CurrentTileMoves = 2;

	//CurrentSelectedAttack.BasePower = 1;
	//CurrentSelectedAttack.BaseRange = 3;
	//CurrentSelectedAttack.Name = "Kick";
}


void ACharacter_Pathfinder::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}


// ------------------------- Base
void ACharacter_Pathfinder::BeginPlayWorkaroundFunction()
{
	CursorToWorld_DynamicMaterial = UMaterialInstanceDynamic::Create(CursorToWorld->GetMaterial(0), this);
	CursorToWorld->SetMaterial(0, CursorToWorld_DynamicMaterial);

	ActorSelected_DynamicMaterial = UMaterialInstanceDynamic::Create(ActorSelected->GetMaterial(0), this);
	ActorSelected->SetMaterial(0, ActorSelected_DynamicMaterial);

	// Snap Actor to Grid
	// The Z Value needs to be retained or else the character will probably clip through the floor
	//FVector OriginalActorLocation = GetActorLocation();
	FVector ActorLocationSnappedToGrid = GetActorLocation().GridSnap(200.f);
	ActorLocationSnappedToGrid.Z = GetActorLocation().Z;
	SetActorLocation(ActorLocationSnappedToGrid);

	// Create Avatar Battle Data WidgetComponent
	if (AvatarBattleDataComponent_Class) {
		AvatarBattleDataComponent_Reference = Cast<UWidgetComponent_AvatarBattleData>(AvatarBattleData_Component->GetUserWidgetObject());

		if (AvatarBattleDataComponent_Reference->IsValidLowLevel()) {
			AvatarBattleDataComponent_Reference->LinkedAvatar = this;
			AvatarBattleDataComponent_Reference->SetAvatarData();
			AvatarBattleDataComponent_Reference->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			AvatarBattleData_Component->DestroyComponent();
		}
	}

	// Set Individual Avatar's Data
	FString ContextString;
	if (AvatarDataTableValue.DataTable->IsValidLowLevel()) {
		AvatarData = *AvatarDataTableValue.GetRow<FAvatar_Struct>(ContextString);
	}


	// Randomize Stats
	//AvatarData.BaseStats.HealthPoints = FMath::RandRange(50, 150);
	//AvatarData.BaseStats.ManaPoints = FMath::RandRange(25, 75);
	//AvatarData.BaseStats.Attack = FMath::RandRange(5, 15);
	//AvatarData.BaseStats.Defence = FMath::RandRange(5, 15);
	//AvatarData.BaseStats.Speed = FMath::RandRange(5, 15);
	//AvatarData.BaseStats.Evade = FMath::RandRange(5, 15);
	//AvatarData.BaseStats.Power = FMath::RandRange(5, 15);
	//AvatarData.MaximumTileMoves = FMath::RandRange(3, 7);

	CurrentHealthPoints = AvatarData.BaseStats.HealthPoints;
	CurrentManaPoints = AvatarData.BaseStats.ManaPoints;
	CurrentTileMoves = AvatarData.MaximumTileMoves;
}


// ------------------------- Cursor
void ACharacter_Pathfinder::OnAvatarCursorOverBegin()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (ActorSelected && ActorSelected_DynamicMaterial) {
		// When the player hovers over an actor they aren't controlling

		if (PlayerControllerReference->CurrentSelectedAvatar != this) {

			// Switch colours based on player's click command
			switch (PlayerControllerReference->PlayerClickMode)
			{
			case (E_PlayerCharacter_ClickModes::E_MoveCharacter):
				ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Blue);
				break;
			case (E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack):
				ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Red);
				break;
			case (E_PlayerCharacter_ClickModes::E_SelectCharacterToControl):
				ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Yellow);
				break;
			default:
				ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::White);
				break;
			}

			ActorSelected->SetWorldLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, 1));
			ActorSelected->SetVisibility(true);
		}
	}

	// Show AvatarBattleDataWidget
	if (AvatarBattleDataComponent_Reference)
		AvatarBattleDataComponent_Reference->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}


void ACharacter_Pathfinder::OnAvatarCursorOverEnd()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// When the player hovers over an actor they aren't controlling
	if (ActorSelected && PlayerControllerReference->CurrentSelectedAvatar != this)
		ActorSelected->SetVisibility(false);

	// Hide AvatarBattleDataWidget
	if (AvatarBattleDataComponent_Reference)
		AvatarBattleDataComponent_Reference->SetVisibility(ESlateVisibility::Collapsed);
}


void ACharacter_Pathfinder::OnAvatarClicked()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (ActorSelected && ActorSelected_DynamicMaterial) {
		ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Red);
		ActorSelected->SetWorldLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, 1));
		ActorSelected->SetVisibility(true);
		CursorToWorld->SetVisibility(true);

		PlayerControllerReference->CurrentSelectedAvatar = this;

		for (TObjectIterator<ACharacter_Pathfinder> Itr; Itr; ++Itr) {
			ACharacter_Pathfinder* FoundActor = *Itr;

			if (PlayerControllerReference->CurrentSelectedAvatar != FoundActor) {
				FoundActor->ActorSelected->SetVisibility(false);
				FoundActor->CursorToWorld->SetVisibility(false);
			}
		}
	}
}


// ------------------------- Battle
void ACharacter_Pathfinder::ShowAttackRange()
{
	FVector TraceSphereStartLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 55);
	FVector TraceSphereEndLocation = FVector(TraceSphereStartLocation.X, TraceSphereStartLocation.Y, 56);
	int TraceSphereRadius = (100 * (CurrentSelectedAttack.BaseRange * 2));
	int CapsuleHalfHeight = 300;
	int TraceSphereSegments = 35;
	float TraceDrawTime = 2.5f;
	const TArray<AActor*> TraceActorsToIgnore;
	TArray<FHitResult> TraceHitResultArray;
	FHitResult TraceHitResult;
	bool TraceComplex = false;
	bool TraceIgnoreSelf = true;
	FLinearColor TraceColour = FLinearColor::Red;
	FLinearColor TraceHitColour = FLinearColor::Green;
	//EDrawDebugTrace::ForDuration;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceTypeQuery;
	TraceTypeQuery.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	bool Hit;

	Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), TraceSphereStartLocation, TraceSphereEndLocation, TraceSphereRadius, UEngineTypes::ConvertToTraceType(ECC_Pawn), TraceComplex,
	TraceActorsToIgnore, EDrawDebugTrace::ForDuration, TraceHitResultArray, true, TraceColour, TraceHitColour, TraceDrawTime);

	//// X-Directional Attack
	//for (int i = 1; i < CurrentSelectedAttack.BaseRange + 1; i++) {
	//	// North
	//	/*Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), FVector(TraceSphereStartLocation.X + (200 * i), TraceSphereStartLocation.Y, TraceSphereStartLocation.Z),
	//		FVector(TraceSphereEndLocation.X + (200 * i), TraceSphereEndLocation.Y, TraceSphereEndLocation.Z), TraceSphereRadius, UEngineTypes::ConvertToTraceType(ECC_Pawn), TraceComplex,
	//		TraceActorsToIgnore, EDrawDebugTrace::ForDuration, TraceHitResultArray, TraceIgnoreSelf, TraceColour, TraceHitColour, TraceDrawTime);*/

	//	Hit = UKismetSystemLibrary::CapsuleTraceMulti(GetWorld(), FVector(TraceSphereStartLocation.X + (200 * i), TraceSphereStartLocation.Y, TraceSphereStartLocation.Z),
	//		FVector(TraceSphereEndLocation.X + (200 * i), TraceSphereEndLocation.Y, TraceSphereEndLocation.Z), TraceSphereRadius, CapsuleHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Pawn), 
	//		TraceComplex, TraceActorsToIgnore, EDrawDebugTrace::ForDuration, TraceHitResultArray, TraceIgnoreSelf, TraceColour, TraceHitColour, TraceDrawTime);
	//}

	if (Hit) {
		for (int i = 0; i < TraceHitResultArray.Num(); i++) {
			if (TraceHitResultArray[i].Actor->GetClass() == this->GetClass() && TraceHitResultArray[i].Actor != this) {
				ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResultArray[i].Actor));
			}
		}
	}

	for (int i = 0; i < ValidAttackTargetsArray.Num(); i++) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Hit Actor: %s"), *ValidAttackTargetsArray[i]->GetName()));
	}
}


void ACharacter_Pathfinder::LaunchAttack(ACharacter_Pathfinder* Target)
{
	FAvatar_UltimateTypeChart* MoveTypeChartRow;
	FAvatar_UltimateTypeChart* TargetTypeChartRow;
	FString ContextString, MoveTypeAsString, TargetTypeAsString;
	float CurrentDamage = CurrentSelectedAttack.BasePower;

	MoveTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(CurrentSelectedAttack.Type).ToString();
	TargetTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(Target->AvatarData.PrimaryType).ToString();

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Launch Attack")));

	// Check for type advantage or disadvantage
	//if (UltimateTypeChartDataTable) {
	MoveTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*MoveTypeAsString), ContextString);
	TargetTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*TargetTypeAsString), ContextString);
	//}

	// Compare each Move type against the Target type
	//for (int i = 0; i < MoveTypeChartRow->CombinationTypes.Num(); i++) {
	for (int j = 0; j < TargetTypeChartRow->CombinationTypes.Num(); j++) {
		// 2x Damage
		if (MoveTypeChartRow->DoesMoreDamageToTypes.Contains(TargetTypeChartRow->CombinationTypes[j])) {
			CurrentDamage = CurrentDamage * 2;
		}
		// 0.5x Damage
		else if (MoveTypeChartRow->DoesLessDamageToTypes.Contains(TargetTypeChartRow->CombinationTypes[j])) {
			CurrentDamage = CurrentDamage / 2;
		}
	}
	//}

	Target->CurrentHealthPoints -= FMath::Clamp<int>((AvatarData.BaseStats.Attack + CurrentDamage) - Target->AvatarData.BaseStats.Defence, 1, 999999999);

	// End this Avatar's turn
	GetWorld()->GetGameState<AStarmark_GameState>()->AvatarEndTurn();

	// Reset Decals
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	PlayerControllerReference->UpdateSelectedAvatar();
}