#include "Starmark_GameMode.h"

#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "PlayerPawn_Static.h"
#include "Actor_GridTile.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Widget_HUD_Battle.h"


// ------------------------- Battle
void AStarmark_GameMode::OnPlayerPostLogin(APlayerController_Base* NewPlayerController)
{
	// Spawn and posses player pawn
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

	MultiplayerUniqueIDCounter++;
	NewPlayerController->MultiplayerUniqueID = MultiplayerUniqueIDCounter;

	PlayerControllerReferences.Add(NewPlayerController);

	// When all players have joined, begin running the functions needed to start the battle
	if (PlayerControllerReferences.Num() >= 2)
		Server_BeginMultiplayerBattle_Implementation();
}


void AStarmark_GameMode::Server_BeginMultiplayerBattle_Implementation()
{
	AStarmark_GameState* GameStateReference = nullptr;
	GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
		Server_SpawnAvatar(PlayerControllerReferences[i]);
		GameStateReference->SetTurnOrder(PlayerControllerReferences);
	}

	Server_MultiplayerBattleCheckAllPlayersReady();
}


void AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady_Implementation()
{
	FString AssembledTurnOrderText;
	TArray<bool> ReadyStatuses;
	AStarmark_GameState* GameStateReference = nullptr;
	GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
		ReadyStatuses.Add(PlayerControllerReferences[i]->IsReadyToStartMultiplayerBattle);
	}

	if (ReadyStatuses.Contains(false)) {
		GetWorld()->GetTimerManager().SetTimer(PlayerReadyCheckTimerHandle, this, &AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady, 1.f, false);
	} else {
		for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
			Cast<AStarmark_GameState>(GetWorld()->GetGameState())->SetTurnOrder(PlayerControllerReferences);

			// Assemble turn order text
			AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GameStateReference->AvatarTurnOrder[i]->PlayerControllerReference->PlayerState);
			PlayerStateReference->Client_UpdateReplicatedPlayerName();

			if (PlayerStateReference)
				AssembledTurnOrderText.Append(GameStateReference->AvatarTurnOrder[i]->AvatarData.AvatarName + " / " + PlayerStateReference->GetPlayerName() + "\n");
			else
				AssembledTurnOrderText.Append(GameStateReference->AvatarTurnOrder[i]->AvatarData.AvatarName + "\n");
		}

		GameStateReference->CurrentTurnOrderText = AssembledTurnOrderText;

		for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
			PlayerControllerReferences[i]->SetBattleWidgetVariables();
		}

		PlayerControllerReferences[1]->UpdateAvatarsDecalsAndWidgets(Cast<AStarmark_GameState>(GetWorld()->GetGameState())->AvatarTurnOrder[0]);
	}
}


void AStarmark_GameMode::Server_SpawnAvatar_Implementation(APlayerController_Base* PlayerController)
{
	FActorSpawnParameters SpawnInfo;
	TArray<AActor*> FoundGridTiletActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTiletActors);

	FVector Location = FoundGridTiletActors[FMath::RandRange(0, FoundGridTiletActors.Num() - 1)]->GetActorLocation();	
	Location.Z = 95;

	ACharacter_Pathfinder* NewAvatarActor = GetWorld()->SpawnActor<ACharacter_Pathfinder>(AvatarBlueprintClass, Location, FRotator::ZeroRotator, SpawnInfo);
	NewAvatarActor->PlayerControllerReference = PlayerController;
	NewAvatarActor->MultiplayerControllerUniqueID = PlayerController->MultiplayerUniqueID;

	PlayerController->CurrentSelectedAvatar = NewAvatarActor;

	//if (HasAuthority())
	PlayerController->OnRepNotify_CurrentSelectedAvatar();
}


// Implemented in Blueprints
void AStarmark_GameMode::EndOfBattle_Implementation()
{
	
}