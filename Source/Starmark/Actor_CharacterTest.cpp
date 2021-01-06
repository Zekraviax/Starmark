#include "Actor_CharacterTest.h"


// Sets default values
AActor_CharacterTest::AActor_CharacterTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Static Mesh Component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AActor_CharacterTest::BeginPlay()
{
	Super::BeginPlay();

	// Dynamic Material
	DynamicMaterial = UMaterialInstanceDynamic::Create(StaticMaterial, this);
	Mesh->SetMaterial(0, DynamicMaterial);

	// Random Colour Test
	//FLinearColor RandColor = FLinearColor(FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f), 1.f);
	//DynamicMaterial->SetVectorParameterValue("Colour2", RandColor);
}

// Called every frame
void AActor_CharacterTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}