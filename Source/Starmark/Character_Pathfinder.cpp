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
#include "Actor_GridTile.h"
#include "PlayerController_Base.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "AttackEffects_FunctionLibrary.h"
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

	// Multiplayer
	bReplicates = true; 
	bReplicateMovement = true; 
	//bReplicateInstigator = true; 
	bNetUseOwnerRelevancy = true;
}


void ACharacter_Pathfinder::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 

	DOREPLIFETIME(ACharacter_Pathfinder, AvatarData);
	DOREPLIFETIME(ACharacter_Pathfinder, PlayerControllerReference);
	DOREPLIFETIME(ACharacter_Pathfinder, AllKnownAttacks);
}



void ACharacter_Pathfinder::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}


// ------------------------- Base
void ACharacter_Pathfinder::BeginPlayWorkaroundFunction_Implementation(FAvatar_Struct NewAvatarData, UWidget_HUD_Battle* BattleHUDReference)
{
	FString ContextString;

	//AvatarData = NewAvatarData;

	CursorToWorld_DynamicMaterial = UMaterialInstanceDynamic::Create(CursorToWorld->GetMaterial(0), this);
	CursorToWorld->SetMaterial(0, CursorToWorld_DynamicMaterial);

	ActorSelected_DynamicMaterial = UMaterialInstanceDynamic::Create(ActorSelected->GetMaterial(0), this);
	ActorSelected->SetMaterial(0, ActorSelected_DynamicMaterial);

	// Hitbox for unusually sized Avatars
	//SetRootComponent(Mesh);
	//CapsuleComponent->AttachTo(RootComponent);

	// Snap Actor to Grid
	// The Z Value needs to be retained or else the character will probably clip through the floor
	FVector ActorLocationSnappedToGrid = GetActorLocation().GridSnap(200.f);
	ActorLocationSnappedToGrid.Z = GetActorLocation().Z;
	SetActorLocation(ActorLocationSnappedToGrid);

	// Set Individual Avatar's Data
	//if (AvatarData.AvatarName == "Default") {
	//	AvatarData = *AvatarDataTableValue.GetRow<FAvatar_Struct>(ContextString);
	//}

	// Add Simple Attacks First, then Other Attacks
	if (AvatarData.SimpleAttacks.Num() > 0) {
		for (int i = 0; i < AvatarData.SimpleAttacks.Num(); i++) {
			if (i < 2) {
				AllKnownAttacks.Add(*AvatarData.SimpleAttacks[i].GetRow<FAvatar_AttackStruct>(ContextString));
			}
		}
	}

	for (int i = 0; i < AvatarData.AttacksLearnedByBuyingWithEssence.Num(); i++) {
		if (AllKnownAttacks.Num() < 4) {
			AllKnownAttacks.Add(*AvatarData.AttacksLearnedByBuyingWithEssence[i].GetRow<FAvatar_AttackStruct>(ContextString));
		}
	}

	CurrentHealthPoints = AvatarData.BaseStats.HealthPoints;
	CurrentManaPoints = AvatarData.BaseStats.ManaPoints;
	CurrentTileMoves = AvatarData.MaximumTileMoves;

	if (AvatarData.AttacksLearnedByBuyingWithEssence.Num() > 0)
		CurrentSelectedAttack = *AvatarData.AttacksLearnedByBuyingWithEssence[0].GetRow<FAvatar_AttackStruct>(ContextString);

	// Create Avatar Battle Data WidgetComponent
	if (AvatarBattleDataComponent_Class) {
		AvatarBattleDataComponent_Reference = Cast<UWidgetComponent_AvatarBattleData>(AvatarBattleData_Component->GetUserWidgetObject());

		if (AvatarBattleDataComponent_Reference->IsValidLowLevel()) {
			AvatarBattleDataComponent_Reference->LinkedAvatar = AvatarData;
			AvatarBattleDataComponent_Reference->UpdateAvatarData(AvatarData);
			AvatarBattleDataComponent_Reference->SetVisibility(ESlateVisibility::Collapsed);
		}
		else {
			//AvatarBattleData_Component->DestroyComponent();
		}
	}

	//if (PlayerControllerReference) {
	//	//AvatarData = NewAvatarData;
	//	//PlayerControllerReference->BattleHUDCodeReference->AvatarBattleDataWidget->LinkedAvatar = this;
	//	PlayerControllerReference->UpdateBattleWidget(BattleHUDReference);
	//}
	//else {
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("BattleHUDReference Not Valid")));
	//}

	//if (BattleHUDReference) {
	//	//AvatarData = NewAvatarData;
	//	BattleHUDReference->AvatarBattleDataWidget->LinkedAvatar = this;
	//	//PlayerControllerReference->UpdateBattleWidget(BattleHUDReference);
	//}
	//else {
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("BattleHUDReference Not Valid")));
	//}

	// Set Skeletal Mesh
	//if (SkeletalMeshReference && Mesh) {

	//}
}


// ------------------------- Cursor
void ACharacter_Pathfinder::OnAvatarCursorOverBegin()
{
	//if (!PlayerControllerReference)
	//	PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (ActorSelected && ActorSelected_DynamicMaterial) {
		// When the player hovers over an actor they aren't controlling
		//if (PlayerControllerReference->CurrentSelectedAvatar != this) {

			// Switch colours based on player's click command
			//switch (PlayerControllerReference->PlayerClickMode)
			//{
			//case (E_PlayerCharacter_ClickModes::E_MoveCharacter):
			//	ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Blue);
			//	break;
			//case (E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack):
			//	CursorToWorld_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Red);
			//	break;
			//case (E_PlayerCharacter_ClickModes::E_SelectCharacterToControl):
			//	CursorToWorld_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Yellow);
			//	break;
			//default:
				CursorToWorld_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Green);
			//	break;
			//}

			ActorSelected->SetWorldLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, 1));
			ActorSelected->SetVisibility(true);
		//}
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

	if (ActorSelected && CursorToWorld_DynamicMaterial) {
		CursorToWorld_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Red);
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
	FVector TraceStartLocation, TraceEndLocation;
	int TraceSphereRadius = (100 * (CurrentSelectedAttack.BaseRange * 2)), CapsuleHalfHeight = 300, TraceSphereSegments = 35;
	float TraceDrawTime = 12.5f;
	const TArray<AActor*> TraceActorsToIgnore;
	TArray<FHitResult> TraceHitResultArray;
	FHitResult TraceHitResult;
	bool TraceComplex = false, TraceIgnoreSelf = true, Hit;
	FLinearColor TraceColour = FLinearColor::Red, TraceHitColour = FLinearColor::Green;
	//EDrawDebugTrace::ForDuration;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceTypeQuery;
	TraceTypeQuery.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	
	// Circle Trace
	if (CurrentSelectedAttack.AttackPattern == EBattle_AttackPatterns::Circle) {
		TraceStartLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 55);
		TraceEndLocation = FVector(TraceStartLocation.X, TraceStartLocation.Y, 56);

		Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), TraceStartLocation, TraceEndLocation, TraceSphereRadius, UEngineTypes::ConvertToTraceType(ECC_Pawn), TraceComplex,
			TraceActorsToIgnore, EDrawDebugTrace::ForDuration, TraceHitResultArray, true, TraceColour, TraceHitColour, TraceDrawTime);

		if (Hit) {
			for (int i = 0; i < TraceHitResultArray.Num(); i++) {
				if (TraceHitResultArray[i].Actor->GetClass() == this->GetClass() && TraceHitResultArray[i].Actor != this) {
					ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResultArray[i].Actor));
					//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ValidAttackTargets: %d"), ValidAttackTargetsArray.Num()));
				}
			}
		}
	}
	// Four-Way Cross Trace
	else if (CurrentSelectedAttack.AttackPattern == EBattle_AttackPatterns::FourWayCross || CurrentSelectedAttack.AttackPattern == EBattle_AttackPatterns::EightWayCross) {
		// North
		TraceStartLocation = FVector(GetActorLocation().X + 200, GetActorLocation().Y, GetActorLocation().Z);
		TraceEndLocation = FVector(TraceStartLocation.X + (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Y, TraceStartLocation.Z);

		Hit = GetWorld()->LineTraceSingleByObjectType(TraceHitResult, TraceStartLocation, TraceEndLocation, FCollisionObjectQueryParams(TraceTypeQuery));
		DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(50.f, 50.f, 250.f) / 1.5f, FColor::Red, false, TraceDrawTime);

		if (Hit) {
			if (TraceHitResult.Actor->IsValidLowLevel())
				if (Cast<ACharacter_Pathfinder>(TraceHitResult.Actor)->IsValidLowLevel())
					ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResult.Actor));
		}

		// South
		TraceStartLocation = FVector(GetActorLocation().X - 200, GetActorLocation().Y, GetActorLocation().Z);
		TraceEndLocation = FVector(GetActorLocation().X - (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Y, TraceStartLocation.Z);

		Hit = GetWorld()->LineTraceSingleByObjectType(TraceHitResult, TraceStartLocation, TraceEndLocation, FCollisionObjectQueryParams(TraceTypeQuery));
		DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(50.f, 50.f, 250.f) / 1.5f, FColor::Red, false, TraceDrawTime);

		if (Hit) {
			if (TraceHitResult.Actor->IsValidLowLevel())
				if (Cast<ACharacter_Pathfinder>(TraceHitResult.Actor)->IsValidLowLevel())
					ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResult.Actor));
		}

		// East
		TraceStartLocation = FVector(GetActorLocation().X, GetActorLocation().Y - 200, GetActorLocation().Z);
		TraceEndLocation = FVector(TraceStartLocation.X, TraceStartLocation.Y - (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Z);

		Hit = GetWorld()->LineTraceSingleByObjectType(TraceHitResult, TraceStartLocation, TraceEndLocation, FCollisionObjectQueryParams(TraceTypeQuery));
		DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(50.f, 50.f, 250.f) / 1.5f, FColor::Red, false, TraceDrawTime);

		if (Hit) {
			if (TraceHitResult.Actor->IsValidLowLevel())
				if (Cast<ACharacter_Pathfinder>(TraceHitResult.Actor)->IsValidLowLevel())
					ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResult.Actor));
		}

		// West
		TraceStartLocation = FVector(GetActorLocation().X, GetActorLocation().Y + 200, GetActorLocation().Z);
		TraceEndLocation = FVector(TraceStartLocation.X, TraceStartLocation.Y + (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Z);

		Hit = GetWorld()->LineTraceSingleByObjectType(TraceHitResult, TraceStartLocation, TraceEndLocation, FCollisionObjectQueryParams(TraceTypeQuery));
		DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(50.f, 50.f, 250.f) / 1.5f, FColor::Red, false, TraceDrawTime);

		if (Hit) {
			if (TraceHitResult.Actor->IsValidLowLevel())
				if (Cast<ACharacter_Pathfinder>(TraceHitResult.Actor)->IsValidLowLevel())
					ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResult.Actor));
		}

		// Eight-Way Cross
		if (CurrentSelectedAttack.AttackPattern == EBattle_AttackPatterns::EightWayCross) {
			// North-West
			TraceStartLocation = FVector(GetActorLocation().X + 200, GetActorLocation().Y + 200, GetActorLocation().Z);
			TraceEndLocation = FVector(TraceStartLocation.X + (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Y + (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Z);

			Hit = GetWorld()->LineTraceSingleByObjectType(TraceHitResult, TraceStartLocation, TraceEndLocation, FCollisionObjectQueryParams(TraceTypeQuery));
			DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(50.f, 50.f, 250.f) / 1.5f, FColor::Red, false, TraceDrawTime);

			if (Hit) {
				if (TraceHitResult.Actor->IsValidLowLevel())
					if (Cast<ACharacter_Pathfinder>(TraceHitResult.Actor)->IsValidLowLevel())
						ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResult.Actor));
			}

			// South-West
			TraceStartLocation = FVector(GetActorLocation().X - 200, GetActorLocation().Y + 200, GetActorLocation().Z);
			TraceEndLocation = FVector(TraceStartLocation.X - (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Y + (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Z);

			Hit = GetWorld()->LineTraceSingleByObjectType(TraceHitResult, TraceStartLocation, TraceEndLocation, FCollisionObjectQueryParams(TraceTypeQuery));
			DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(50.f, 50.f, 250.f) / 1.5f, FColor::Red, false, TraceDrawTime);

			if (Hit) {
				if (TraceHitResult.Actor->IsValidLowLevel())
					if (Cast<ACharacter_Pathfinder>(TraceHitResult.Actor)->IsValidLowLevel())
						ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResult.Actor));
			}

			// South-East
			TraceStartLocation = FVector(GetActorLocation().X - 200, GetActorLocation().Y - 200, GetActorLocation().Z);
			TraceEndLocation = FVector(TraceStartLocation.X - (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Y - (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Z);

			Hit = GetWorld()->LineTraceSingleByObjectType(TraceHitResult, TraceStartLocation, TraceEndLocation, FCollisionObjectQueryParams(TraceTypeQuery));
			DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(50.f, 50.f, 250.f) / 1.5f, FColor::Red, false, TraceDrawTime);

			if (Hit) {
				if (TraceHitResult.Actor->IsValidLowLevel())
					if (Cast<ACharacter_Pathfinder>(TraceHitResult.Actor)->IsValidLowLevel())
						ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResult.Actor));
			}

			// North-East
			TraceStartLocation = FVector(GetActorLocation().X + 200, GetActorLocation().Y - 200, GetActorLocation().Z);
			TraceEndLocation = FVector(TraceStartLocation.X + (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Y - (200 * CurrentSelectedAttack.BaseRange), TraceStartLocation.Z);

			Hit = GetWorld()->LineTraceSingleByObjectType(TraceHitResult, TraceStartLocation, TraceEndLocation, FCollisionObjectQueryParams(TraceTypeQuery));
			DrawDebugBox(GetWorld(), TraceHitResult.Location, FVector(50.f, 50.f, 250.f) / 1.5f, FColor::Red, false, TraceDrawTime);

			if (Hit) {
				if (TraceHitResult.Actor->IsValidLowLevel())
					if (Cast<ACharacter_Pathfinder>(TraceHitResult.Actor)->IsValidLowLevel())
						ValidAttackTargetsArray.AddUnique(Cast<ACharacter_Pathfinder>(TraceHitResult.Actor));
			}
		}
	}

	for (int i = 0; i < ValidAttackTargetsArray.Num(); i++) {
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Hit Actor: %s"), *ValidAttackTargetsArray[i]->GetName()));
	}
}


void ACharacter_Pathfinder::LaunchAttack_Implementation(ACharacter_Pathfinder* Target)
{
	FAvatar_UltimateTypeChart* MoveTypeChartRow;
	FAvatar_UltimateTypeChart* TargetTypeChartRow;
	FString ContextString, MoveTypeAsString, TargetTypeAsString;
	int CurrentDamage = 1;

	MoveTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(CurrentSelectedAttack.Type).ToString();
	TargetTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(Target->AvatarData.PrimaryType).ToString();

	// Check for mana
	//if (CurrentManaPoints >= CurrentSelectedAttack.ManaCost) {
	//	CurrentManaPoints -= CurrentSelectedAttack.ManaCost;
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Enough Mana")));
	//}
	//else {
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Not Enough Mana")));
	//	return;
	//}

	// Check for type advantage or disadvantage
	MoveTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*MoveTypeAsString), ContextString);
	TargetTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*TargetTypeAsString), ContextString);

	// Use Gamemode's attack function
	//Cast<AStarmark_GameState>(GetWorld()->GetGameState())->GameState_LaunchAttack_Implementation(this, Target);
	//Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->GameMode_LaunchAttack_Implementation(this, Target);

	// Attack Damage Formula
	/*CurrentDamage = FMath::CeilToInt(AvatarData.BaseStats.Attack * CurrentSelectedAttack.BasePower);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 1 = %d"), CurrentDamage));
	CurrentDamage = FMath::CeilToInt(CurrentDamage / Target->AvatarData.BaseStats.Defence);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 2 = %d"), CurrentDamage));
	CurrentDamage = FMath::CeilToInt((AvatarData.BaseStats.Power / 2) * CurrentDamage);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 3 = %d"), CurrentDamage));
	CurrentDamage = FMath::CeilToInt(CurrentDamage / 8);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 3 = %d"), CurrentDamage));*/

	// Compare each Move type against the Target type
	//for (int j = 0; j < TargetTypeChartRow->CombinationTypes.Num(); j++) {
	//	// 2x Damage
	//	if (MoveTypeChartRow->DoesMoreDamageToTypes.Contains(TargetTypeChartRow->CombinationTypes[j])) {
	//		CurrentDamage = CurrentDamage * 2;
	//	}
	//	// 0.5x Damage
	//	else if (MoveTypeChartRow->DoesLessDamageToTypes.Contains(TargetTypeChartRow->CombinationTypes[j])) {
	//		CurrentDamage = CurrentDamage / 2;
	//	}
	//}

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Final Damage  = %d"), CurrentDamage));

	// Subtract Health
	//if (Target) {
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Target Valid")));
	//	Target->AvatarData.CurrentHealthPoints -= CurrentDamage;
	//	Target->UpdatePlayerBattleHUD();
	//} else {
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Target Not Valid")));
	//}

	// Update AvatarDataWidgets
	//Target->PlayerControllerReference->HUD


	// Apply move effects after the damage has been dealt
	for (int i = 0; i < CurrentSelectedAttack.AttackEffectsOnTarget.Num(); i++) {
		UAttackEffects_FunctionLibrary::SwitchOnAttackEffect(CurrentSelectedAttack.AttackEffectsOnTarget[i], this, Target);
	}

	// End this Avatar's turn
	//GetWorld()->GetGameState<AStarmark_GameState>()->AvatarEndTurn();
}
//void ACharacter_Pathfinder::LaunchAttack(ACharacter_Pathfinder* Target)
//{
//	FAvatar_UltimateTypeChart* MoveTypeChartRow;
//	FAvatar_UltimateTypeChart* TargetTypeChartRow;
//	FString ContextString, MoveTypeAsString, TargetTypeAsString;
//	int CurrentDamage = 1;
//
//	MoveTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(CurrentSelectedAttack.Type).ToString();
//	TargetTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(Target->AvatarData.PrimaryType).ToString();
//
//	// Check for mana
//	if (CurrentManaPoints >= CurrentSelectedAttack.ManaCost) {
//		CurrentManaPoints -= CurrentSelectedAttack.ManaCost;
//	}
//	else {
//		return;
//	}
//
//	// Check for type advantage or disadvantage
//	MoveTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*MoveTypeAsString), ContextString);
//	TargetTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*TargetTypeAsString), ContextString);
//
//	// Attack Damage Formula
//	CurrentDamage = FMath::CeilToInt(AvatarData.BaseStats.Attack * CurrentSelectedAttack.BasePower);
//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 1 = %d"), CurrentDamage));
//	CurrentDamage = FMath::CeilToInt(CurrentDamage / Target->AvatarData.BaseStats.Defence);
//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 2 = %d"), CurrentDamage));
//	CurrentDamage = FMath::CeilToInt((AvatarData.BaseStats.Power / 2) * CurrentDamage);
//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 3 = %d"), CurrentDamage));
//	CurrentDamage = FMath::CeilToInt(CurrentDamage / 8);
//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 3 = %d"), CurrentDamage));
//
//	// Compare each Move type against the Target type
//	for (int j = 0; j < TargetTypeChartRow->CombinationTypes.Num(); j++) {
//		// 2x Damage
//		if (MoveTypeChartRow->DoesMoreDamageToTypes.Contains(TargetTypeChartRow->CombinationTypes[j])) {
//			CurrentDamage = CurrentDamage * 2;
//		}
//		// 0.5x Damage
//		else if (MoveTypeChartRow->DoesLessDamageToTypes.Contains(TargetTypeChartRow->CombinationTypes[j])) {
//			CurrentDamage = CurrentDamage / 2;
//		}
//	}
//
//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage Final = %d"), CurrentDamage));
//	// Subtract Health
//	Target->AvatarData.CurrentHealthPoints -= CurrentDamage;
//
//	// Apply move effects after the damage has been dealt
//	for (int i = 0; i < CurrentSelectedAttack.AttackEffectsOnTarget.Num(); i++) {
//		UAttackEffects_FunctionLibrary::SwitchOnAttackEffect(CurrentSelectedAttack.AttackEffectsOnTarget[i], this, Target);
//	}
//
//	// End this Avatar's turn
//	GetWorld()->GetGameState<AStarmark_GameState>()->AvatarEndTurn();
//}


void ACharacter_Pathfinder::SetTilesOccupiedBySize()
{
	FHitResult LineTraceResult;
	TEnumAsByte<EObjectTypeQuery> ObjectToTrace = EObjectTypeQuery::ObjectTypeQuery1;
	TArray<TEnumAsByte<EObjectTypeQuery>>ObjectsToTraceAsByte;
	ObjectsToTraceAsByte.Add(ObjectToTrace);
	FVector End = FVector(GetActorLocation().X, GetActorLocation().Y, 0.f);
	FVector Start = GetActorLocation();
	bool SuccessfulLineTrace;

	for (int i = 0; i < AvatarData.OccupiedTiles.Num(); i++) {
		//for (int y = 0; y < AvatarData.Size.SizeY; y++) {

		End = FVector(Start.X + (200 * AvatarData.OccupiedTiles[i].X), Start.Y + (200 * AvatarData.OccupiedTiles[i].Y), 0.f);
		SuccessfulLineTrace = GetWorld()->LineTraceSingleByObjectType(LineTraceResult, Start, End, FCollisionObjectQueryParams(ObjectsToTraceAsByte));

		//if (AvatarData.OccupiedTiles.Num() > 1) {
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Line Trace at Location: %s"), *Start.ToString()));
		//DrawDebugBox(GetWorld(), End, FVector(50.f, 50.f, 100.f) / 1.5f, FColor::Red, false, 2.5f);
		//}

		if (SuccessfulLineTrace) {
			Cast<AActor_GridTile>(LineTraceResult.Actor)->TraversalProperties.AddUnique(E_GridTile_TraversalProperties::E_Occupied);

			if (Cast<AActor_GridTile>(LineTraceResult.Actor)->TraversalProperties.Contains(E_GridTile_TraversalProperties::E_None)) {
				Cast<AActor_GridTile>(LineTraceResult.Actor)->TraversalProperties.Remove(E_GridTile_TraversalProperties::E_None);
			}
		}
		//}
	}
}


void ACharacter_Pathfinder::UpdatePlayerBattleHUD()
{
	if (PlayerControllerReference) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("PlayerControllerReference Valid")));
		//PlayerControllerReference->BattleHUDCodeReference->AvatarBattleDataWidget->UpdateAvatarData(AvatarData);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("PlayerControllerReference Not Valid")));
	}
}