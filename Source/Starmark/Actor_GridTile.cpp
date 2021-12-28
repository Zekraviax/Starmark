#include "Actor_GridTile.h"


#include "Character_Pathfinder.h"
#include "Starmark_GameState.h"


// Sets default values
AActor_GridTile::AActor_GridTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize Components
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(RootComponent);
	Floor->SetRelativeTransform(FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(2.f, 2.f, 1.f)));

	// Hitbox
	GridTileHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("GridTileHitbox"));
	GridTileHitbox->SetupAttachment(RootComponent);
	GridTileHitbox->SetRelativeScale3D(FVector(2.5f, 2.5f, 1.f));
	// Set 'Simulation Generates Hit Events'
	GridTileHitbox->SetNotifyRigidBodyCollision(true);
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
	TArray<AActor*> OverlappingActors;
	GridTileHitbox->GetOverlappingActors(OverlappingActors, ACharacter_Pathfinder::StaticClass());

	if (this) {
		// Clear TraversalProperties that aren't permanent (e.g. Occupied by an Avatar)
		if (OverlappingActors.Num() <= 0)
			Properties.Remove(E_GridTile_Properties::E_Occupied);
	}

	//// If the TraversalProperties array is empty, add the default Property
	if (Properties.Num() <= 0)
		Properties.AddUnique(E_GridTile_Properties::E_None);
}


void AActor_GridTile::OnMouseBeginHover()
{
	// Set AttackTraceActor to this tile's location if it isn't centered around the player
	// Get the current Avatar
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());
	ACharacter_Pathfinder* CurrentAvatar = GameStateReference->DynamicAvatarTurnOrder[GameStateReference->CurrentAvatarTurnIndex];

	if (CurrentAvatar->CurrentSelectedAttack.AttackPattern == EBattle_AttackPatterns::AOE_Circle) {
		if (CurrentAvatar->PlayerControllerReference) {
			CurrentAvatar->AttackTraceActor->SetWorldLocation(FVector(GetActorLocation().X, GetActorLocation().Y + 200, CurrentAvatar->AttackTraceActor->GetComponentLocation().Z), true, nullptr, ETeleportType::ResetPhysics);
		}
	}
}


void AActor_GridTile::OnMouseEndHover()
{

}


void AActor_GridTile::UpdateTileColour(E_GridTile_ColourChangeContext ColourChangeContext)
{
	if (DynamicMaterial->IsValidLowLevel()) {
		switch (ColourChangeContext)
		{
		case (E_GridTile_ColourChangeContext::Normal):
			// Heirarcy of colours based on factors such as tile properties
			// Lowest priority: White (no properties that change colour)
			DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::White);
			// Road (free travel)
			if (Properties.Contains(E_GridTile_Properties::E_StoneRoad))
				DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor(0.5f, 0.2f, 0.f, 1.f));
			// Shadow
			if (Properties.Contains(E_GridTile_Properties::Shadow))
				DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor(0.1f, 0.1f, 0.1f, 1.f));
			// Highest priority: Tile is un-traversable
			if (Properties.Contains(E_GridTile_Properties::E_Wall) ||
				Properties.Contains(E_GridTile_Properties::E_Occupied))
				DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor(0.5f, 0.5f, 0.5f, 1.f));
				break;
		case (E_GridTile_ColourChangeContext::OnMouseHover):
			DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Green);
			break;
		case (E_GridTile_ColourChangeContext::OnMouseHoverTileUnreachable):
			DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Red);
			break;
		case (E_GridTile_ColourChangeContext::WithinAttackRange):
			DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor(1.f, 0.2f, 0.f, 1.f));
			break;
		default:
			break;
		}
	}
}