#include "Character_NonAvatarEntity.h"


#include "Actor_GridTile.h"
#include "AIController_Avatar.h"
#include "Character_Pathfinder.h"


ACharacter_NonAvatarEntity::ACharacter_NonAvatarEntity()
{
	// Spawn 'Mesh'
	CubeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube Component"));
	CubeComponent->AttachTo(RootComponent);
}


// ------------------------- Hurricane
void ACharacter_NonAvatarEntity::HurricaneOnSpawn()
{
	ActorSelectedPlane->DestroyComponent();
	ActorSelected->DestroyComponent();
	AttackTraceActor->DestroyComponent();
	AvatarBattleData_Component->DestroyComponent();
	GetMesh()->DestroyComponent();

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	EntityType = E_NonAvatarEntity_EntityType::Hurricane;
}


void ACharacter_NonAvatarEntity::HurricaneOnAvatarCollision(ACharacter_Pathfinder* Avatar)
{
	// Get all grid tiles that aren't occupied
	TArray<AActor*> GridTilesArray;
	AActor_GridTile* LaunchLocation = nullptr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);

	// Discard all tiles that are occupied or un-traversable
	for (int i = GridTilesArray.Num() - 1; i >= 0; i--) {
		AActor_GridTile* FoundTile = Cast<AActor_GridTile>(GridTilesArray[i]);

		if (FoundTile->Properties.Contains(E_GridTile_Properties::E_Occupied) ||
			FoundTile->Properties.Contains(E_GridTile_Properties::E_Wall)) {
			GridTilesArray.RemoveAt(i);
		} else if (FoundTile->GetActorLocation().X == GetActorLocation().X &&
			FoundTile->GetActorLocation().Y == GetActorLocation().Y) {
			GridTilesArray.RemoveAt(i);
		}
	}

	if (GridTilesArray.Num() >= 1) {
		LaunchLocation = Cast<AActor_GridTile>(GridTilesArray[FMath::RandRange(0, (GridTilesArray.Num() - 1))]);
		FVector LaunchVelocity = FVector::ZeroVector;

		Avatar->SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, Avatar->GetActorLocation().Z));

		LaunchVelocity = LaunchLocation->GetActorLocation() - Avatar->GetActorLocation().GridSnap(200.f);
		LaunchVelocity.Z = 400.f;

		Cast<AAIController_Avatar>(Avatar->GetController())->StopMovement();

		Avatar->LaunchCharacter(LaunchVelocity, false, true);
		Avatar->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

		// set a delay before restoring collisions
		BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}