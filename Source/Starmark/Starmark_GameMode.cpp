#include "Starmark_GameMode.h"

#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "PlayerPawn_Static.h"
#include "Actor_GridTile.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"


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

		// Initialize avatars on the server
		//for (int j = 0; j < GameStateReference->AvatarTurnOrder.Num(); j++) {
		//	Cast<AStarmark_PlayerState>(GameStateReference->AvatarTurnOrder[j]->PlayerControllerReference->PlayerState)->PlayerState_BeginBattle();
		//	GameStateReference->AvatarTurnOrder[j]->BeginPlayWorkaroundFunction();
		//}

		GameStateReference->CurrentTurnOrderText = AssembledTurnOrderText;

		for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
			PlayerControllerReferences[i]->SetBattleWidgetVariables();
		}

		Server_UpdateAllAvatarDecals();

		// Set first player to act
		GameStateReference->AvatarTurnOrder[0]->PlayerControllerReference->IsCurrentlyActingPlayer = true;
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

	// Spawn DynamicMaterial on server

	NewAvatarActor->ActorSelected_DynamicMaterial = UMaterialInstanceDynamic::Create(NewAvatarActor->ActorSelected->GetMaterial(0), this);
	NewAvatarActor->ActorSelected->SetMaterial(0, NewAvatarActor->ActorSelected_DynamicMaterial);
	//NewAvatarActor->ActorSelected_DynamicMaterial->SetIsReplicated(true);
	NewAvatarActor->ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Blue);
	
	NewAvatarActor->PlayerControllerReference = PlayerController;
	NewAvatarActor->MultiplayerControllerUniqueID = PlayerController->MultiplayerUniqueID;

	PlayerController->CurrentSelectedAvatar = NewAvatarActor;
	PlayerController->OnRepNotify_CurrentSelectedAvatar();
}


void AStarmark_GameMode::Server_UpdateAllAvatarDecals_Implementation()
{
	TArray<AActor*> Avatars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), Avatars);
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());
	ACharacter_Pathfinder* CurrentlyActingAvatar = GameStateReference->AvatarTurnOrder[GameStateReference->CurrentAvatarTurnIndex];

	for (int i = 0; i < Avatars.Num(); i++) {
		ACharacter_Pathfinder* FoundActor = Cast<ACharacter_Pathfinder>(Avatars[i]);

		if (FoundActor->AvatarBattleDataComponent_Reference->IsValidLowLevel())
			FoundActor->AvatarBattleDataComponent_Reference->SetVisibility(ESlateVisibility::Collapsed);

		//if (FoundActor == CurrentlyActingAvatar) {
		//	//if (FoundActor->MultiplayerControllerUniqueID == CurrentlyActingAvatar->MultiplayerControllerUniqueID)
		//	FoundActor->ActorSelected_DynamicMaterial_Colour = FLinearColor::Green;
		//	//else
		//	//	FoundActor->ActorSelected_DynamicMaterial_Colour = FLinearColor::Red;
		//} else
		//	FoundActor->ActorSelected_DynamicMaterial_Colour = FLinearColor::Red;

		//FoundActor->ActorSelectedDynamicMaterialColourUpdate();
	}

	for (int j = 0; j < PlayerControllerReferences.Num(); j++)
		PlayerControllerReferences[j]->UpdateAvatarDecals(CurrentlyActingAvatar);
}


// Implemented in Blueprints
void AStarmark_GameMode::EndOfBattle_Implementation()
{
	
}