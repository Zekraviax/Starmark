#include "Actor_GridTile.h"


#include "Character_Pathfinder.h"


// Sets default values
AActor_GridTile::AActor_GridTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize Components
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(RootComponent);
	Floor->SetRelativeTransform(FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(2.f, 2.f, 1.f)));
}

// Called when the game starts or when spawned
void AActor_GridTile::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterial = UMaterialInstanceDynamic::Create(Floor->GetMaterial(0), this);
	Floor->SetMaterial(0, DynamicMaterial);
}

// Called every frame
void AActor_GridTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// ------------------------- Battle
void AActor_GridTile::UpdateGridTileState()
{
	// Clear TraversalProperties that aren't permanent (e.g. Occupied by an Avatar)
	Properties.Remove(E_GridTile_Properties::E_Occupied);

	// Line Trace for an Avatar occupying this tile
	FHitResult LineTraceResult;
	TEnumAsByte<EObjectTypeQuery> ObjectToTrace = EObjectTypeQuery::ObjectTypeQuery3;
	FVector End = FVector(GetActorLocation().X, GetActorLocation().Y, (GetActorLocation().Z + 100.f));

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsToTraceAsByte;
	ObjectsToTraceAsByte.Add(ObjectToTrace);

	bool SuccessfulLineTrace = GetWorld()->LineTraceSingleByObjectType(LineTraceResult, GetActorLocation(), End, FCollisionObjectQueryParams(ObjectsToTraceAsByte));

	// Tell the Avatar to update Tiles based on its Size
	if (SuccessfulLineTrace)
		Cast<ACharacter_Pathfinder>(LineTraceResult.Actor)->SetTilesOccupiedBySize();

	// If the TraversalProperties array is empty, add the default Property
	if (Properties.Num() <= 0)
		Properties.AddUnique(E_GridTile_Properties::E_None);
}


void AActor_GridTile::OnMouseBeginHover()
{
	if (DynamicMaterial->IsValidLowLevel())
		DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Green);
}


void AActor_GridTile::OnMouseEndHover()
{
	if (DynamicMaterial->IsValidLowLevel())
		DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::White);
}