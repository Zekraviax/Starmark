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
#include "Kismet/GameplayStatics.h"
#include "PlayerController_Base.h"

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

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
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
	ActorSelected->SetVisibility(false);
}


// ------------------------- Cursor
void ACharacter_Pathfinder::OnAvatarCursorOverBegin()
{
	if (!PlayerControllerReference) {
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}

	if (ActorSelected && ActorSelected_DynamicMaterial) {
		if (PlayerControllerReference->CurrentSelectedPawn != this) {
			ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Blue);
			ActorSelected->SetWorldLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, 1));
			ActorSelected->SetVisibility(true);
		}
	}
}


void ACharacter_Pathfinder::OnAvatarCursorOverEnd()
{
	if (!PlayerControllerReference) {
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}

	if (ActorSelected && PlayerControllerReference->CurrentSelectedPawn != this)
		ActorSelected->SetVisibility(false);

	//if (ActorSelected && PlayerControllerReference->CurrentSelectedPawn != this)
	//	ActorSelected->SetVisibility(false);
}


void ACharacter_Pathfinder::OnAvatarClicked()
{
	if (!PlayerControllerReference) {
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}

	if (ActorSelected && ActorSelected_DynamicMaterial) {
		ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Red);
		ActorSelected->SetWorldLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, 1));
		ActorSelected->SetVisibility(true);
		CursorToWorld->SetVisibility(true);

		PlayerControllerReference->CurrentSelectedPawn = this;

		for (TObjectIterator<ACharacter_Pathfinder> Itr; Itr; ++Itr) {
			ACharacter_Pathfinder* FoundActor = *Itr;

			if (PlayerControllerReference->CurrentSelectedPawn != FoundActor) {
				FoundActor->ActorSelected->SetVisibility(false);
				FoundActor->CursorToWorld->SetVisibility(false);
			}
		}
	}
}