#include "Character_Pathfinder.h"

#include "Actor_GridTile.h"
#include "Actor_WorldGrid.h"
#include "AIController_Avatar.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController_Battle.h"
#include "Starmark_GameState.h"
#include "WidgetComponent_AvatarBattleData.h"


ACharacter_Pathfinder::ACharacter_Pathfinder()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 69.0f);

	// Don't rotate character to camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to face direction they move
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1000.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Actor Selected Decal (Don't delete this)
	ActorHighlightedDecal = CreateDefaultSubobject<UDecalComponent>("ActorHighlightedDecal");
	ActorHighlightedDecal->SetupAttachment(RootComponent);
	ActorHighlightedDecal->SetVisibility(true);
	ActorHighlightedDecal->SetHiddenInGame(false);
	ActorHighlightedDecal->DecalSize = FVector(32.0f, 64.0f, 64.0f);
	//ActorHighlightedDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	ActorHighlightedDecal->SetRelativeScale3D(FVector(1.25f, 1.f, 2.f));

	// Actor Selected Plane
	ActorSelectedPlane = CreateDefaultSubobject<UStaticMeshComponent>("ActorSelectedPlane");
	ActorSelectedPlane->SetupAttachment(RootComponent);
	ActorSelectedPlane->SetVisibility(true);
	ActorSelectedPlane->SetHiddenInGame(false);

	// Attack Trace Actor Component
	AttackTraceActor = CreateDefaultSubobject<UStaticMeshComponent>("AttackTraceActor");
	AttackTraceActor->SetupAttachment(RootComponent);
	AttackTraceActor->SetVisibility(true);
	AttackTraceActor->SetHiddenInGame(false);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//Hitbox Component
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetVisibility(true);
	BoxComponent->SetHiddenInGame(false);
	BoxComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 3.f));
	// Set 'Simulation Generates Hit Events'
	BoxComponent->SetNotifyRigidBodyCollision(true);

	// Multiplayer
	bReplicates = true; 
	//bReplicateMovement = true; 
	bNetUseOwnerRelevancy = true;
	IndexInPlayerParty = -1;
	//ActorSelected->SetIsReplicated(true);
	ActorSelectedPlane->SetIsReplicated(true);
}


// Replicated variables
void ACharacter_Pathfinder::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 

	DOREPLIFETIME(ACharacter_Pathfinder, AvatarData);
	DOREPLIFETIME(ACharacter_Pathfinder, PlayerControllerReference);
	DOREPLIFETIME(ACharacter_Pathfinder, CurrentKnownAttacks);
	DOREPLIFETIME(ACharacter_Pathfinder, CurrentSelectedAttack);
	DOREPLIFETIME(ACharacter_Pathfinder, IndexInPlayerParty);
}


void ACharacter_Pathfinder::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


// ------------------------- Base
void ACharacter_Pathfinder::BeginPlayWorkaroundFunction_Implementation(UWidget_HUD_Battle* BattleHUDReference)
{
	// Snap Actor to Grid
	// The Z Value needs to be retained or else the character will probably clip through the floor
	FVector ActorLocationSnappedToGrid = GetActorLocation().GridSnap(200.f);
	ActorLocationSnappedToGrid.Z = GetActorLocation().Z;
	SetActorLocation(ActorLocationSnappedToGrid);

	AvatarData.CurrentHealthPoints = AvatarData.BattleStats.MaximumHealthPoints;
	AvatarData.CurrentManaPoints = AvatarData.BattleStats.MaximumManaPoints;
	AvatarData.CurrentTileMoves = AvatarData.MaximumTileMoves;

	// Set default selected attack
	//if (CurrentKnownAttacks.Num() > 0)
	//	CurrentSelectedAttack = CurrentKnownAttacks[0];

	IndexInPlayerParty = 0;

	// Highlight dynamic material
	ActorHighlightedDecalMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(ActorHighlightMaterial, this);
	ActorHighlightedDecal->SetMaterial(0, ActorHighlightedDecalMaterialInstanceDynamic);
}


// ------------------------- Cursor
void ACharacter_Pathfinder::OnAvatarCursorOverBegin()
{
	UE_LOG(LogTemp, Warning, TEXT("OnAvatarCursorOverBegin / Cursor over begin on actor: %s"), *GetName());

	if (ActorSelectedPlane)
		ActorSelectedPlane->SetWorldLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, 1));

	if (!AvatarBattleDataComponent_Reference)
		AvatarBattleDataComponent_Reference = Cast<UWidgetComponent_AvatarBattleData>(AvatarBattleData_Component->GetUserWidgetObject());

	AvatarBattleDataComponent_Reference->LinkedAvatar = AvatarData;
	AvatarBattleDataComponent_Reference->UpdateAvatarData(AvatarData);

	AvatarBattleData_Component->SetHiddenInGame(false);

	ActorHighlightedDecal->SetVisibility(true);
}


void ACharacter_Pathfinder::OnAvatarCursorOverEnd()
{
	AvatarBattleData_Component->SetHiddenInGame(true);

	ActorHighlightedDecal->SetVisibility(false);
}


void ACharacter_Pathfinder::OnAvatarClicked()
{
	if (!PlayerControllerReference) {
		PlayerControllerReference = Cast<APlayerController_Battle>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}

	PlayerControllerReference->CurrentSelectedAvatar = this;

	if (ActorSelectedPlane) {
		ActorSelectedPlane->SetWorldLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, 1));

		for (TObjectIterator<ACharacter_Pathfinder> Itr; Itr; ++Itr) {
			ACharacter_Pathfinder* FoundActor = *Itr;

			if (PlayerControllerReference->CurrentSelectedAvatar != FoundActor) {
				FoundActor->ActorSelectedPlane->SetHiddenInGame(false);
			}
		}
	}
}


// ------------------------- Battle
void ACharacter_Pathfinder::SetActorHighlightProperties(bool IsVisible, E_GridTile_ColourChangeContext ColourChangeContext)
{
	if (ActorHighlightedDecal->IsValidLowLevel() && ActorHighlightedDecalMaterialInstanceDynamic->IsValidLowLevel()) {
		ActorHighlightedDecal->SetVisibility(IsVisible);

		switch (ColourChangeContext)
		{
		case (E_GridTile_ColourChangeContext::Normal):
			// Heirarcy of colours based on factors such as tile properties
			// Lowest priority: White (no properties that change colour)
			ActorHighlightedDecalMaterialInstanceDynamic->SetVectorParameterValue("Colour", FLinearColor(1.f, 1.f, 1.f, 1.f));
			break;
		case (E_GridTile_ColourChangeContext::OnMouseHover):
			ActorHighlightedDecalMaterialInstanceDynamic->SetVectorParameterValue("Colour", FLinearColor(0.f, 1.f, 0.f, 1.f));
			break;
		case (E_GridTile_ColourChangeContext::OnMouseHoverTileUnreachable):
			ActorHighlightedDecalMaterialInstanceDynamic->SetVectorParameterValue("Colour", FLinearColor(1.f, 0.f, 0.f, 1.f));
			break;
		case (E_GridTile_ColourChangeContext::WithinAttackRange):
			ActorHighlightedDecalMaterialInstanceDynamic->SetVectorParameterValue("Colour", FLinearColor(1.f, 0.2f, 0.f, 1.f));
			break;
		default:
			break;
		}
	} else {
		//UE_LOG(LogTemp, Warning, TEXT("ACharacter_Pathfinder / SetActorHighlightProperties / ActorHighlightDecal is not valid."));
	}
}


void ACharacter_Pathfinder::GetValidActorsForAttack_Implementation(FAvatar_AttackStruct Attack, AActor* CurrentlyHoveredActor)
{
	TArray<FVector2D> ValidVectors;
	TArray<AActor*> GridTilesArray, EntitiesArray, WorldGridActorsArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StaticClass(), EntitiesArray);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), WorldGridActorsArray);
	AActor_WorldGrid* WorldGrid = Cast<AActor_WorldGrid>(WorldGridActorsArray[0]);

	ValidAttackTargetsArray.Empty();

	//UE_LOG(LogTemp, Warning, TEXT("ACharacter_Pathfinder / GetValidActorsForAttack / Attack pattern: %s"), *UEnum::GetDisplayValueAsText(Attack.AttackPattern).ToString());
	switch (Attack.AttackPattern)
	{
	case(EBattle_AttackPatterns::SingleTile):
		if (Cast<AActor_GridTile>(CurrentlyHoveredActor) || Cast<ACharacter_Pathfinder>(CurrentlyHoveredActor)) {
			ValidAttackTargetsArray.Add(CurrentlyHoveredActor);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("ACharacter_Pathfinder / GetValidActorsForAttack / Error when getting valid targets for SingleTarget attack."));
		}

		break;
	case(EBattle_AttackPatterns::FourWayCross):
		for (int i = 1; i <= Attack.BaseRange; i++) {
			ValidVectors.Add(FVector2D(this->GetActorLocation().X + (200 * i), this->GetActorLocation().Y));
			ValidVectors.Add(FVector2D(this->GetActorLocation().X - (200 * i), this->GetActorLocation().Y));
			ValidVectors.Add(FVector2D(this->GetActorLocation().X, this->GetActorLocation().Y + (200 * i)));
			ValidVectors.Add(FVector2D(this->GetActorLocation().X, this->GetActorLocation().Y - (200 * i)));
		}

		for (AActor* GridTile : GridTilesArray) {
			for (FVector2D Vector : ValidVectors) {
				if (FMath::IsNearlyEqual(Vector.X, GridTile->GetActorLocation().X, 2) && FMath::IsNearlyEqual(Vector.Y, GridTile->GetActorLocation().Y, 2)) {
					ValidAttackTargetsArray.AddUnique(GridTile);
					break;
				}
			}
		}

		for (AActor* Entity : EntitiesArray) {
			for (FVector2D Vector : ValidVectors) {
				if (FMath::IsNearlyEqual(Vector.X, Entity->GetActorLocation().X, 2) && FMath::IsNearlyEqual(Vector.Y, Entity->GetActorLocation().Y, 2)) {
					ValidAttackTargetsArray.AddUnique(Entity);
					break;
				}
			}
		}

		break;
	case(EBattle_AttackPatterns::EightWayCross):
		for (int i = 1; i <= Attack.BaseRange; i++) {
			// Cardinal direction tiles
			ValidVectors.Add(FVector2D(this->GetActorLocation().X + (200 * i), this->GetActorLocation().Y));
			ValidVectors.Add(FVector2D(this->GetActorLocation().X - (200 * i), this->GetActorLocation().Y));
			ValidVectors.Add(FVector2D(this->GetActorLocation().X, this->GetActorLocation().Y + (200 * i)));
			ValidVectors.Add(FVector2D(this->GetActorLocation().X, this->GetActorLocation().Y - (200 * i)));

			// Diagonal tiles
			ValidVectors.Add(FVector2D(this->GetActorLocation().X + (200 * i), this->GetActorLocation().Y + (200 * i)));
			ValidVectors.Add(FVector2D(this->GetActorLocation().X + (200 * i), this->GetActorLocation().Y - (200 * i)));
			ValidVectors.Add(FVector2D(this->GetActorLocation().X - (200 * i), this->GetActorLocation().Y - (200 * i)));
			ValidVectors.Add(FVector2D(this->GetActorLocation().X - (200 * i), this->GetActorLocation().Y + (200 * i)));
		}

		for (AActor* GridTile : GridTilesArray) {
			if (ValidVectors.Contains(FVector2D(GridTile->GetActorLocation().X, GridTile->GetActorLocation().Y))) {
				ValidAttackTargetsArray.Add(GridTile);
			}
		}

		for (AActor* Entity : EntitiesArray) {
			if (ValidVectors.Contains(FVector2D(Entity->GetActorLocation().X, Entity->GetActorLocation().Y))) {
				ValidAttackTargetsArray.Add(Entity);
			}
		}
		
		break;
	case(EBattle_AttackPatterns::Circle):
		for (AActor* GridTile : GridTilesArray) {
			if (!GetActorLocation().Equals(GridTile->GetActorLocation(), 11)) {
				if (WorldGrid->GetTotalDistanceBetweenTwoPositions(GetActorLocation(), GridTile->GetActorLocation()) <= Attack.BaseRange) {
					ValidAttackTargetsArray.Add(GridTile);
				}
			}
		}

		for (AActor* Entity : EntitiesArray) {
			if (!GetActorLocation().Equals(Entity->GetActorLocation(), 11)) {
				if (WorldGrid->GetTotalDistanceBetweenTwoPositions(GetActorLocation(), Entity->GetActorLocation()) <= Attack.BaseRange) {
					ValidAttackTargetsArray.Add(Entity);
				}
			}
		}
		
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("ACharacter_Pathfinder / GetValidActorsForAttack / This attack does not have an implemented Pattern!"));
		break;
	}
}


void ACharacter_Pathfinder::LaunchAttack_Implementation(AActor* Target)
{
	// Tell the client to update the server
	UE_LOG(LogTemp, Warning, TEXT("LaunchAttack / Attack chosen: %s"), *CurrentSelectedAttack.Name);
	PlayerControllerReference->Client_SendLaunchAttackToServer(this, Target, CurrentSelectedAttack.Name);
}


void ACharacter_Pathfinder::SetTilesOccupiedBySize(bool ClearTiles)
{
	TArray<AActor*> OverlappingActors;
	FVector Start = GetActorLocation();

	// Clear out tiles that were overlapped
	if (ClearTiles) {
		TArray<AActor*> GridTilesArray;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
		for (AActor* TileInArray : GridTilesArray) {
			AActor_GridTile* ActorAsGridTile = Cast<AActor_GridTile>(TileInArray);

			if (ActorAsGridTile->OccupyingActor == this) {
				ActorAsGridTile->OccupyingActor = nullptr;

				if (ActorAsGridTile->Properties.Contains(E_GridTile_Properties::E_Occupied)) {
					ActorAsGridTile->Properties.Remove(E_GridTile_Properties::E_Occupied);
				}

				if (ActorAsGridTile->Properties.Num() <= 0) {
					ActorAsGridTile->Properties.AddUnique(E_GridTile_Properties::E_None);
				}
			}
		}
	}

	// Set overlapping tiles to 'Occupied'
	for (int j = 0; j < OverlappingActors.Num(); j++) {
		Cast<AActor_GridTile>(OverlappingActors[j])->Properties.AddUnique(E_GridTile_Properties::E_Occupied);
		Cast<AActor_GridTile>(OverlappingActors[j])->OccupyingActor = this;

		UE_LOG(LogTemp, Warning, TEXT("SetTilesOccupiedBySize / Set Tile to be occupied."));

		if (Cast<AActor_GridTile>(OverlappingActors[j])->Properties.Contains(E_GridTile_Properties::E_None)) {
			Cast<AActor_GridTile>(OverlappingActors[j])->Properties.Remove(E_GridTile_Properties::E_None);
		}
	}

	BoxComponent->SetWorldLocation(Start);
}


void ACharacter_Pathfinder::UpdateAvatarDataInPlayerParty()
{
	if (PlayerControllerReference) {
		if (PlayerControllerReference->PlayerParty.IsValidIndex(IndexInPlayerParty)) {
			PlayerControllerReference->PlayerParty[IndexInPlayerParty] = AvatarData;
		} else {
			UE_LOG(LogTemp, Warning, TEXT("ACharacter_Pathfinder / UpdateAvatarDataInPlayerParty / Index in player party %d is not valid"), IndexInPlayerParty);
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("ACharacter_Pathfinder / UpdateAvatarDataInPlayerParty / PlayerControllerReference is not valid"));
	}
}


void ACharacter_Pathfinder::ActorSelectedDynamicMaterialColourUpdate_Implementation() 
{
	// Implemented in Blueprints
}


void ACharacter_Pathfinder::AvatarBeginTileOverlap_Implementation()
{
	// Implemented in Blueprints
}


void ACharacter_Pathfinder::AvatarStopMoving(bool SnapToGrid)
{
	// Make sure this Avatar's position is snapped to the grid
	if (GetActorLocation() != GetActorLocation().GridSnap(200) && SnapToGrid) {
		Cast<AAIController_Avatar>(GetController())->MoveToLocation((GetActorLocation().GridSnap(200)), -1.f, false, false, false, true, 0, false);
	}
}


ECharacter_FacingDirections ACharacter_Pathfinder::GetCharacterFacingDirection()
{
	ECharacter_FacingDirections ReturnDirection = {};

	if (GetActorRotation().Yaw >= -315.f && GetActorRotation().Yaw <= -225.f) {
		// Bottom Right
		ReturnDirection = ECharacter_FacingDirections::BottomRight;
	} else if (GetActorRotation().Yaw >= -225.f && GetActorRotation().Yaw <= -135.f) {
		// Bottom Left
		ReturnDirection = ECharacter_FacingDirections::BottomLeft;
	} else if (GetActorRotation().Yaw >= -135.f && GetActorRotation().Yaw <= -45.f) {
		// Top Left
		ReturnDirection = ECharacter_FacingDirections::TopLeft;
	} else if (GetActorRotation().Yaw >= -45.f && GetActorRotation().Yaw <= 45.f) {
		// Top Right
		ReturnDirection = ECharacter_FacingDirections::TopRight;
	} else if (GetActorRotation().Yaw >= 45.f && GetActorRotation().Yaw <= 135.f) {
		// Bottom Right
		ReturnDirection = ECharacter_FacingDirections::BottomRight;
	} else if (GetActorRotation().Yaw >= 135.f && GetActorRotation().Yaw <= 225.f) {
		// Bottom Left
		ReturnDirection = ECharacter_FacingDirections::BottomLeft;
	} else if (GetActorRotation().Yaw >= 225.f && GetActorRotation().Yaw <= 315.f) {
		// Top Left
		ReturnDirection = ECharacter_FacingDirections::TopLeft;
	} else if (GetActorRotation().Yaw >= 315.f && GetActorRotation().Yaw <= 405.f) {
		// Top Right
		ReturnDirection = ECharacter_FacingDirections::TopRight;
	} else {
		UE_LOG(LogTemp, Warning, TEXT("ACharacter_Pathfinder / GetCharacterFacingDirection() / Error: Character Yaw Value Too High Or Low"));
	}

	return ReturnDirection;
}


// ------------------------- Multiplayer
void ACharacter_Pathfinder::Client_GetAvatarData_Implementation(FAvatar_Struct NewAvatarData)
{
	Local_GetAvatarData(NewAvatarData);
}


void ACharacter_Pathfinder::Local_GetAvatarData(FAvatar_Struct NewAvatarData)
{
	AvatarData = NewAvatarData;
}


void ACharacter_Pathfinder::SynchronizeAvatarDataToGameState_Implementation()
{
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	// Step one: Find the player that controls this avatar.
	for (int i = 0; i < GameStateReference->PlayerDataStructsArray.Num(); i++) {
		if (PlayerControllerReference->PlayerDataStruct.ReplicationID == GameStateReference->PlayerDataStructsArray[i].ReplicationID) {
			// Step two: Find the avatar amongst that players' team.
			for (int j = 0; j < GameStateReference->PlayerDataStructsArray[i].CurrentAvatarTeam.Num(); j++) {
				if (AvatarData.BattleUniqueID == GameStateReference->PlayerDataStructsArray[i].CurrentAvatarTeam[j].BattleUniqueID) {
					// Copy this avatars' data into the array.
					GameStateReference->PlayerDataStructsArray[i].CurrentAvatarTeam[j] = AvatarData;
					break;
				}
			}
			break;
		}
	}
}
