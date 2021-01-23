#include "Actor_GridTile.h"


// Sets default values
AActor_GridTile::AActor_GridTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize Components
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(RootComponent);

	FTransform FloorTransform;
	FloorTransform.SetScale3D(FVector(2.f, 2.f, 1.f));
	Floor->SetRelativeTransform(FloorTransform);

}

// Called when the game starts or when spawned
void AActor_GridTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_GridTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// ------------------------- Grid Tile
void AActor_GridTile::UpdateGridTileState()
{
	// Clear TraversalProperties that aren't permanent (e.g. Occupied)
	TraversalProperties.Remove(E_GridTile_TraversalProperties::E_Occupied);

	// Line Trace for an Avatar occupying this tile
	FHitResult LineTraceResult;
	TEnumAsByte<EObjectTypeQuery> ObjectToTrace = EObjectTypeQuery::ObjectTypeQuery3;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectsToTraceAsByte;
	ObjectsToTraceAsByte.Add(ObjectToTrace);
	FVector End = FVector(GetActorLocation().X, GetActorLocation().Y, (GetActorLocation().Z + 100.f));
	bool SuccessfulLineTrace = GetWorld()->LineTraceSingleByObjectType(LineTraceResult, GetActorLocation(), End, FCollisionObjectQueryParams(ObjectsToTraceAsByte));

	if (SuccessfulLineTrace) {
		//GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Green, FString::Printf(TEXT("Found Actor: %s"), *LineTraceResult.Actor->GetName()));
		TraversalProperties.Add(E_GridTile_TraversalProperties::E_Occupied);
	} 
	//else {
	//	GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, ("Line Trace Failed"));
	//}

	// If the TraversalProperties array is empty, add the default Property
	if (TraversalProperties.Num() <= 0) {
		TraversalProperties.Add(E_GridTile_TraversalProperties::E_None);
	}
}