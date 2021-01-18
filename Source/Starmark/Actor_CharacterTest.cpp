#include "Actor_CharacterTest.h"


#include "PlayerController_Base.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AActor_CharacterTest::AActor_CharacterTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create Static Mesh Component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	// Create Cursor To World Decal
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	//static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Materials/P4T_Assets/CommandRing_Mat.CommandRing_Mat'"));
	//if (DecalMaterialAsset.Succeeded())
	//{
	//	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	//}
	CursorToWorld->SetupAttachment(RootComponent);

	// Create Actor Selected Decal
	ActorSelected = CreateDefaultSubobject<UDecalComponent>("ActorSelected");
	ActorSelected->DecalSize = FVector(32.0f, 64.0f, 64.0f);
	ActorSelected->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	ActorSelected->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AActor_CharacterTest::BeginPlay()
{
	Super::BeginPlay();

	// Dynamic Material
	DynamicMaterial = UMaterialInstanceDynamic::Create(StaticMaterial, this);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
}

// Called every frame
void AActor_CharacterTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CursorToWorld != nullptr)
	{
		if (APlayerController_Base* PC = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		{
			//FHitResult TraceHitResult;
			//PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			//FVector CursorFV = TraceHitResult.ImpactNormal;
			//FRotator CursorR = CursorFV.Rotation();

			// Snap Location To Grid
			//FVector CursorSnappedToGrid = CursorFV.GridSnap(200.f);

			//CursorToWorld->SetWorldLocation(CursorSnappedToGrid);
			//PC->CursorLocationSnappedToGrid = CursorSnappedToGrid;
			//CursorToWorld->SetWorldRotation(CursorR);
		}
	}
}