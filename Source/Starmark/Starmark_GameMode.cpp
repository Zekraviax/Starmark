#include "Starmark_GameMode.h"

#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "PlayerPawn_Static.h"
#include "Actor_GridTile.h"
#include "Starmark_GameState.h"


// ------------------------- Battle
void AStarmark_GameMode::OnPlayerPostLogin(APlayerController_Base* NewPlayerController)
{
	TArray<AActor*> FoundPlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundPlayerStartActors);

	FVector Location;
	FRotator Rotation;
	FActorSpawnParameters SpawnInfo;

	if (FoundPlayerStartActors.IsValidIndex(0)) {
		if (FoundPlayerStartActors[0]->IsValidLowLevel()) {
			Location = FoundPlayerStartActors[0]->GetActorLocation();
			Rotation = FoundPlayerStartActors[0]->GetActorRotation();
		}
	}

	NewPlayerController->Possess(GetWorld()->SpawnActor<APlayerPawn_Static>(PlayerPawnBlueprintClass, Location, Rotation, SpawnInfo));

	PlayerControllerReferences.Add(NewPlayerController);
	if (PlayerControllerReferences.Num() >= 2)
		Server_BeginMultiplayerBattle_Implementation();
}


void AStarmark_GameMode::Server_BeginMultiplayerBattle_Implementation()
{
	for (int i = 0; i < PlayerControllerReferences.Num(); i++)
		Server_SpawnAvatar_Implementation(PlayerControllerReferences[i]);

	Cast<AStarmark_GameState>(GetWorld()->GetGameState())->SetTurnOrder_Implementation(PlayerControllerReferences);
}


void AStarmark_GameMode::Server_SpawnAvatar_Implementation(APlayerController_Base* PlayerController)
{
	TArray<AActor*> FoundGridTiletActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTiletActors);

	FVector Location = FoundGridTiletActors[FMath::RandRange(0, FoundGridTiletActors.Num() - 1)]->GetActorLocation();
	Location.Z = 100;
	FActorSpawnParameters SpawnInfo;

	ACharacter_Pathfinder* NewAvatarActor = GetWorld()->SpawnActor<ACharacter_Pathfinder>(AvatarBlueprintClass, Location, FRotator::ZeroRotator, SpawnInfo);

	NewAvatarActor->PlayerControllerReference = PlayerController;
	PlayerController->CurrentSelectedAvatar = NewAvatarActor;

	if (HasAuthority())
		PlayerController->OnRepNotify_CurrentSelectedAvatar();
}


// Implemented in Blueprints
void AStarmark_GameMode::EndOfBattle_Implementation()
{
	
}