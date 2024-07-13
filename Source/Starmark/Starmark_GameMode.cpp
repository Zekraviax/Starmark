#include "Starmark_GameMode.h"

#include "Actor_AttackEffectsLibrary.h"
#include "Actor_GridTile.h"
#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "PlayerController_Battle.h"
#include "PlayerController_Lobby.h"
#include "PlayerPawn_Flying.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameMode_Lobby.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Starmark_Variables.h"
#include "Widget_HUD_Battle.h"


// ------------------------- Local Helper Functions
void AStarmark_GameMode::SetGameStateLocalReference()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / SetGameStateLocalReference / Finding a valid GameStateReference..."));

	TArray<AActor*> FoundGameStates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameState::StaticClass(), FoundGameStates);
	
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / SetGameStateLocalReference / Found game states: %d"), FoundGameStates.Num());
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / SetGameStateLocalReference / First game state: %s"), *FoundGameStates[0]->GetName());
	GameStateReference = Cast<AStarmark_GameState>(FoundGameStates[0]);
}


UStarmark_GameInstance* AStarmark_GameMode::GetHostPlayerGameStateInstanceReference() const
{
	// To-Do: Figure out how to always get the host player
	// (Each player instance can keep track of whether they're the host?)
	//if (!HostPlayerGameInstanceReference) {}

	return HostPlayerGameInstanceReference;
}


// ------------------------- Battle
void AStarmark_GameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / Player has finished loading"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / AController type: %s"), *C->GetName());
	APlayerController_Battle* NewController = Cast<APlayerController_Battle>(SpawnPlayerController(ROLE_Authority, FVector(0, 0, 0), FRotator(0, 0, 0)));

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / Attempting to swap player controllers"));
	SwapPlayerControllers(Cast<APlayerController>(C), NewController);

	// Set-up for the new controller
	NewController->CreateBattleWidget();

	// Set-up for the battle
	OnPlayerPostLogin(NewController);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / End function"));
}


// ------------------------- Battle
// OnPlayerPostLogin isn't called when seamlessly travelling, because the clients aren't disconnected
void AStarmark_GameMode::OnPlayerPostLogin(APlayerController_Battle* NewPlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Begin function"));

	// The one true method of sending the players' data to the server.
	NewPlayerController->ClientSendDataToServer();
	PlayerControllerReferences.Add(NewPlayerController);

	// Spawn and posses player pawn
	TArray<AActor*> FoundPlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundPlayerStartActors);
	FVector Location;
	FRotator Rotation;
	FActorSpawnParameters SpawnInfo;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Search for valid spawn points for the player pawn"));
	if (FoundPlayerStartActors.IsValidIndex(0)) {
		if (FoundPlayerStartActors[0]->IsValidLowLevel()) {
			Location = FoundPlayerStartActors[0]->GetActorLocation();
			Rotation = FoundPlayerStartActors[0]->GetActorRotation();

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Found a location for the player pawn to spawn"));
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / FoundPlayerStartActors[0] is not valid"));
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / FoundPlayerStartActors.IsValidIndex(0) is not valid"));
	}

	NewPlayerController->Possess(GetWorld()->SpawnActor<APlayerPawn_Flying>(PlayerPawnBlueprintClass, Location, Rotation, SpawnInfo));
	NewPlayerController->ClientSetRotation(Rotation, true);
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Player controller possessed pawn: %s"), *NewPlayerController->GetPawn()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Player pawn rotation: %s"), *Rotation.ToString());

	// We're assigning each player a unique number just in case two or more players have the same player name
	MultiplayerUniqueIDCounter++;
	NewPlayerController->MultiplayerUniqueID = MultiplayerUniqueIDCounter;
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / MultiplayerUniqueIDCounter is: %d"), MultiplayerUniqueIDCounter);

	// Clear Combat Log
	if (CombatLogTextArray.Num() > 0) {
		CombatLogTextArray.Empty();
	}

	// Clear the Server Lobby widget from the players' display
	NewPlayerController->Client_ClearLobbyFromScreen();

	// When all players have joined, begin running the setup functions needed to start the battle.
	// To-Do: Add a step here to check if a session even exists before trying to fetch the number of connections.
	if (!HostPlayerGameInstanceReference) {
		// This should only be done once per battle.
		HostPlayerGameInstanceReference = Cast<UStarmark_GameInstance>(NewPlayerController->GetGameInstance());
	}

	// Here is where we need to check if a multiplayer session exists.
	// If a session does not exist, then it is safe to assume the player is in a singleplayer battle.
	if (HostPlayerGameInstanceReference->DoesSessionExist()) {
		ExpectedPlayers = HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPrivateConnections + HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPublicConnections;

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected private connections: %d"), HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPrivateConnections);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected public connections: %d"), HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPublicConnections);
	} else {
		ExpectedPlayers = 1;
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Total PlayerControllerReferences in array: %d"), PlayerControllerReferences.Num());
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected players: %d"), ExpectedPlayers);

	// To-Do: Figure out why the ExpectedPlayers is being overridden to 1.
	// Don't call this function unless all PlayerStates have their player data.
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Start checking if all players are ready"));
	GetWorld()->GetTimerManager().SetTimer(PlayerReadyCheckTimerHandle, this, &AStarmark_GameMode::GetPreBattleChecks, 1.f, false);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / End function"));
}


void AStarmark_GameMode::GetPreBattleChecks_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Checking if all players have readied up"));
	
	bool AreAllPlayersReady = true;

	// Can't fetch the GameState here, it just crashes.
	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	if (GameStateReference) {
		for (int i = 0; i < GameStateReference->PlayerArray.Num(); i++) {
			if (GameStateReference->PlayerArray.IsValidIndex(i)) {
				AStarmark_PlayerState* FoundPlayerState = Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i]);

				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Checking ready status from player: %s"), *Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->PlayerData.PlayerName);
				
				if (FoundPlayerState->PreBattleCheck == false) {
					//AreAllPlayersReady = false;
					//break;
				}
			} else {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / GameStateReference at index %d is not valid."), i);
			}
		}
	} else {
		AreAllPlayersReady = false;
	}
	
	if (AreAllPlayersReady) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Checking whether or not all players have traveled. Expected players: %d"), ExpectedPlayers);
		if (ExpectedPlayers >= 2) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Starting a multiplayer battle."));

			Server_BeginMultiplayerBattle();
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Starting a single player battle."));

			Server_BeginMultiplayerBattle();
		}

	} else {
		GetWorld()->GetTimerManager().SetTimer(PlayerReadyCheckTimerHandle, this, &AStarmark_GameMode::GetPreBattleChecks, 1.f, false);
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / End function"));
}


void AStarmark_GameMode::Server_BeginMultiplayerBattle_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Begin function"));
	
	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	for (int i = 0; i < GameStateReference->PlayerDataStructsArray.Num(); i++) {
		FPlayer_Data& PlayerData = GameStateReference->PlayerDataStructsArray[i];
		TArray<FAvatar_Struct>& CurrentPlayerTeam = PlayerData.CurrentAvatarTeam;
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Double checking player name: %s"), *PlayerData.PlayerName);

		int SpawnedAvatarCount = 0;
		for (int j = 0; j < CurrentPlayerTeam.Num(); j++) {
			if (CurrentPlayerTeam.IsValidIndex(j)) {
				if (CurrentPlayerTeam[j].AvatarName != "Default") {
					// Upon finding a valid avatar in any players' party,
					// increment the BattleUniqueIDCounter and assign that ID to the avatar.
					BattleUniqueIDCounter++;
					CurrentPlayerTeam[j].BattleUniqueID = BattleUniqueIDCounter;
					
					if (SpawnedAvatarCount < 4) {
						Server_SpawnAvatar(PlayerControllerReferences[i], (j + 1), CurrentPlayerTeam[j]);
						UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Spawn avatar %s for player %s"), *CurrentPlayerTeam[j].AvatarName, *PlayerData.PlayerName);
						
						SpawnedAvatarCount++;
					} else {
						// Calculate variables like Current Health for reserve avatars here.
						CurrentPlayerTeam[j].CurrentHealthPoints = CurrentPlayerTeam[j].SpeciesMinimumStats.MaximumHealthPoints;
						CurrentPlayerTeam[j].CurrentManaPoints = CurrentPlayerTeam[j].SpeciesMinimumStats.MaximumManaPoints;
						CurrentPlayerTeam[j].CurrentTileMoves = CurrentPlayerTeam[j].MaximumTileMoves;
					}
				} else {
					GameStateReference->PlayerDataStructsArray[i].CurrentAvatarTeam.RemoveAt(j);
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Remove invalid member %d from PlayerState_PlayerParty"), j);
				}
			}
		}

		PlayerControllerReferences[i]->OnRepNotify_CurrentSelectedAvatar();
	}
	
	Server_MultiplayerBattleCheckAllPlayersReady();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / End function"));
}


void AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Check all %d PlayerControllers are ready for the battle"), PlayerControllerReferences.Num());
	
	TArray<bool> ReadyStatuses;
	TArray<AActor*> GridTilesArray;

	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}
	
	for (APlayerController_Battle* Controller : GameStateReference->ReturnAllBattlePlayerControllers()) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Is %s ready to start the battle? %s"), *Controller->PlayerDataStruct.PlayerName, Controller->IsReadyToStartMultiplayerBattle ? TEXT("true") : TEXT("false"));

		ReadyStatuses.Add(Controller->IsReadyToStartMultiplayerBattle);
	}

	if (ReadyStatuses.Contains(false) || ReadyStatuses.Num() < ExpectedPlayers) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Can't assemble turn order text because not all players are ready"));
		GetWorld()->GetTimerManager().SetTimer(PlayerReadyCheckTimerHandle, this, &AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady, 1.f, false);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / All players are ready to start"));
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Set the turn order for the avatars"));

		// Only set the turn order once all entities are spawned.
		GameStateReference->SetTurnOrder();

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Fill the DynamicAvatarTurnOrder array for the first time"));
		GameStateReference->DynamicAvatarTurnOrder = GameStateReference->AvatarTurnOrder;
		GameStateReference->OnRepNotify_DynamicAvatarTurnOrderUpdated();

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Assemble the turn order text"));
		Server_AssembleTurnOrderText();

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Assign UniqueIDs and CurrentSelectedAvatars"));
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Update each avatars' attacks in their controllers' HUD"));
		for (APlayerController_Battle* Controller : GameStateReference->ReturnAllBattlePlayerControllers()) {
			for (int j = 0; j < GameStateReference->AvatarTurnOrder.Num(); j++) {
				if (Controller->MultiplayerUniqueID == GameStateReference->AvatarTurnOrder[j]->MultiplayerControllerUniqueID) {
					Controller->CurrentSelectedAvatar = GameStateReference->AvatarTurnOrder[j];
					Controller->Client_UpdateAttacksInHud(GameStateReference->AvatarTurnOrder[j]);
					break;
				}
			}

			Controller->SetBattleWidgetVariables();
		}

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Update all avatar decals"));
		Server_UpdateAllAvatarDecals();
		
		// Set first Avatar's controller as the currently acting player
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Set the currently acting player"));
		GameStateReference->AvatarTurnOrder[0]->PlayerControllerReference->IsCurrentlyActingPlayer = true;

		// Assign helper variables
		// To-Do: Validate this line still works
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Currently acting avatar: %s"), *GameStateReference->ReturnCurrentlyActingAvatar()->AvatarData.Nickname);

		// Initial HUD set up for all players
		GameStateReference->ShowHideAllPlayerHuds();

		
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / End normal functionality. Begin dev tests..."));

		// Currently, this function only sets an avatar's base stats to their minimum, since there is currently no way for them to scale upwards.
		if (GetHostPlayerGameStateInstanceReference()->GetDevSettingsStruct().RecalculateAvatarStatsAtStartOfBattle) {
			TArray<AActor*> Avatars;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), Avatars);
			FString ContextString;

			for (int i = 0; i < Avatars.Num(); i++) {
				ACharacter_Pathfinder* FoundActor = Cast<ACharacter_Pathfinder>(Avatars[i]);
				for (const auto Row : GetHostPlayerGameStateInstanceReference()->GetAvatarsDataTable()->GetRowMap()) {
					const FAvatar_Struct* AvatarData = reinterpret_cast<FAvatar_Struct*>(Row.Value);

					if (AvatarData->AvatarName == FoundActor->AvatarData.AvatarName) {
						FoundActor->AvatarData.BattleStats = AvatarData->SpeciesMinimumStats;
						FoundActor->AvatarData.CurrentHealthPoints = AvatarData->SpeciesMinimumStats.MaximumHealthPoints;
						FoundActor->AvatarData.CurrentManaPoints = AvatarData->SpeciesMinimumStats.MaximumManaPoints;
						break;
					}
				}
			}
		}

		for (APlayerController_Battle* Controller : GameStateReference->ReturnAllBattlePlayerControllers()) {
			Controller->SetBattleWidgetVariables();
			Controller->BattleWidgetReference->SetCurrentActingEntityInfo(GameStateReference->DynamicAvatarTurnOrder[0]);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / End function"));
}


void AStarmark_GameMode::Server_AssembleTurnOrderText_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Assembling text."));
	
	// The AssembleTurnOrderText should only handle the text, and not images in the hud.
	// But whenever the text is updated, shouldn't the images be updated to match?
	FString NewTurnOrderText;

	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	for (int i = 0; i < GameStateReference->AvatarTurnOrder.Num(); i++) {
		if (GameStateReference->AvatarTurnOrder[i]->PlayerControllerReference->IsValidLowLevel()) {
			// To-Do: Differentiate between player controlled entities and enemy entities in the turn order list.
			NewTurnOrderText.Append(GameStateReference->AvatarTurnOrder[i]->AvatarData.Nickname + "\n");
		} else {
			NewTurnOrderText.Append(GameStateReference->AvatarTurnOrder[i]->AvatarData.Nickname + " / ?\n");
		}
	}

	GameStateReference->CurrentTurnOrderText = NewTurnOrderText;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Full turn order text: %s"), *NewTurnOrderText);
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Turn order text assembled"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / End function"));
}


void AStarmark_GameMode::Server_SpawnAvatar_Implementation(APlayerController_Battle* PlayerController, int IndexInPlayerParty, FAvatar_Struct AvatarData, FVector Location = FVector(0, 0, -9999))
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Begin function"));
	
	FString ContextString;
	TArray<AActor*> FoundGridTileActors, ValidMultiplayerSpawnTiles, FoundAvatars;
	TArray<FName> AvatarRowNames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTileActors);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundAvatars);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Attempting to spawn avatar actor"));
	// Find all tiles that can spawn avatars in multiplayer battles, if the player hasn't selected a location.
	for (int i = 0; i < FoundGridTileActors.Num(); i++) {
		const AActor_GridTile* GridTileReference = Cast<AActor_GridTile>(FoundGridTileActors[i]);
		if (Location.Z == -9999) {
			if (GridTileReference->Properties.Contains(E_GridTile_Properties::E_PlayerAvatarSpawn) &&
				GridTileReference->AvatarSlotNumber == IndexInPlayerParty &&
				GridTileReference->AssignedMultiplayerUniqueID == PlayerController->MultiplayerUniqueID) {
				ValidMultiplayerSpawnTiles.Add(FoundGridTileActors[i]);

				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Found valid spawn tile at location: x %d, y %d"),
					FMath::RoundToInt(FoundGridTileActors[i]->GetActorLocation().X),
					FMath::RoundToInt(FoundGridTileActors[i]->GetActorLocation().Y));
				}
		} else if (Location.Z != -9999 && FMath::IsNearlyEqual(GridTileReference->GetActorLocation().X == Location.X, 1.f) && FMath::IsNearlyEqual(GridTileReference->GetActorLocation().Y == Location.Y, 1.f)) {
			// Find the tile and add it to the valid spawn tiles array, so we don't have to refactor this colossal block of code.
			ValidMultiplayerSpawnTiles.Add(FoundGridTileActors[i]);
			break;
		} 
	}


	if (ValidMultiplayerSpawnTiles.Num() > 0) {
		const FActorSpawnParameters SpawnInfo;
		FVector Location = ValidMultiplayerSpawnTiles[0]->GetActorLocation();
		Location.Z = 100;

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Spawn at location: x %d, y %d"), FMath::RoundToInt(Location.X), FMath::RoundToInt(Location.Y));

		ACharacter_Pathfinder* NewAvatarActor = GetWorld()->SpawnActor<ACharacter_Pathfinder>(AvatarBlueprintClass, Location, FRotator::ZeroRotator, SpawnInfo);
		NewAvatarActor->AvatarData = AvatarData;

		// Avatar Stats
		NewAvatarActor->AvatarData.CurrentHealthPoints = NewAvatarActor->AvatarData.BattleStats.MaximumHealthPoints;
		NewAvatarActor->AvatarData.CurrentManaPoints = NewAvatarActor->AvatarData.BattleStats.MaximumManaPoints;
		NewAvatarActor->AvatarData.CurrentTileMoves = NewAvatarActor->AvatarData.MaximumTileMoves;

		// To-Do: Figure out a way for each avatar to have a reference to their players' data
		// without cloning the data for each avatar.
		// (pass-by-reference)
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Give the avatar a copy of the player %s's data"), *PlayerController->PlayerDataStruct.ProfileName);
		
		NewAvatarActor->PlayerControllerReference = PlayerController;

		// To-Do: Clean up the MultiPlayer Unique IDs so that there is only one that is used everywhere.
		NewAvatarActor->MultiplayerControllerUniqueID = PlayerController->MultiplayerUniqueID;
		PlayerController->PlayerDataStruct.CurrentAvatarTeam[IndexInPlayerParty - 1].OwnerMultiplayerUniqueID = PlayerController->MultiplayerUniqueID;
		NewAvatarActor->AvatarData = PlayerController->PlayerDataStruct.CurrentAvatarTeam[IndexInPlayerParty - 1];

		// Get attacks
		UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(GetGameInstance());
		for (int i = 0; i < AvatarData.CurrentEquippedAttackNames.Num(); i++) {
			if (AvatarData.CurrentEquippedAttackNames[i] != FName("None")) {
				NewAvatarActor->CurrentKnownAttacks.Add(*GameInstanceReference->ReturnAttackFromDataTable(AvatarData.CurrentEquippedAttackNames[i]));
			}
		}

		// Sent data to Clients
		// To-Do: Consider sending avatar data to the GameState? Because all players will want to see all avatar data.
		NewAvatarActor->Client_GetAvatarData(NewAvatarActor->AvatarData);

		// To-Do: Deprecate this cloning of avatar data, or just make it a reference to the GameState's data.
		PlayerController->CurrentSelectedAvatar = NewAvatarActor;

		// Set spawn tile to be occupied.
		// ---- Can be refactored into a function ---- //
		if (Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Contains(E_GridTile_Properties::E_None)) {
			Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Remove(E_GridTile_Properties::E_None);
		}

		Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Add(E_GridTile_Properties::E_Occupied);
		Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->OccupyingActor = NewAvatarActor;
		// ---- End refactorable code chunk ---- //

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Avatar spawned"));
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / End function"));
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Failed to find valid tile for Avatar at index: %d"), IndexInPlayerParty);
	}
}


void AStarmark_GameMode::Server_UpdateAllAvatarDecals_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_UpdateAllAvatarDecals / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_UpdateAllAvatarDecals / Updating all avatar decals"));
	
	TArray<AActor*> Avatars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), Avatars);
	ACharacter_Pathfinder* CurrentlyActingAvatar;

	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	// To-Do: Figure out this code chunk is here and whether or not its relevant
	if (GameStateReference->AvatarTurnOrder.IsValidIndex(GameStateReference->CurrentAvatarTurnIndex)) {
		CurrentlyActingAvatar = GameStateReference->AvatarTurnOrder[GameStateReference->CurrentAvatarTurnIndex];
	} else { 
		CurrentlyActingAvatar = GameStateReference->AvatarTurnOrder[0];
	}

	for (int i = 0; i < Avatars.Num(); i++) {
		ACharacter_Pathfinder* FoundActor = Cast<ACharacter_Pathfinder>(Avatars[i]);

		if (IsValid(FoundActor->PlayerControllerReference)) {
			bool IsCurrentlyActing = false;

			if (CurrentlyActingAvatar == Avatars[i]) {
				IsCurrentlyActing = true;
			}

			FoundActor->MultiplayerControllerUniqueID = FoundActor->PlayerControllerReference->MultiplayerUniqueID;

			for (int j = 0; j < PlayerControllerReferences.Num(); j++) {
				PlayerControllerReferences[j]->GetAvatarUpdateFromServer(FoundActor, FoundActor->MultiplayerControllerUniqueID, IsCurrentlyActing, true);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_UpdateAllAvatarDecals / End function"));
}


void AStarmark_GameMode::Server_LaunchAttack_Implementation(ACharacter_Pathfinder* Attacker, AActor* Target, const FString& AttackName)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / Begin function"));
	
	FAvatar_AttackStruct AttackData;
	FString MoveTypeAsString, TargetTypeAsString;
	TArray<FName> ComplexAttackRowNames = AvatarComplexAttacksDataTable->GetRowNames();
	ACharacter_Pathfinder* TargetAsCharacter = Cast<ACharacter_Pathfinder>(Target);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / Attack chosen: %s"), *AttackName);
	
	for (int i = 0; i < ComplexAttackRowNames.Num(); i++)
	{
		const FString ContextString;
		if (AvatarComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttackRowNames[i], ContextString)->Name == AttackName) {
			AttackData = *AvatarComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttackRowNames[i], ContextString);
			break;
		}
	}

	if (Attacker->CurrentSelectedAttack.AttackCategory == EBattle_AttackCategories::Offensive && Attacker->CurrentSelectedAttack.BasePower > 0) {
		// Calculate damage, one step at a time
		float CurrentDamage = FMath::RandRange(1.f, 5.f);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / Damage calculation started with random float: %s"), *FString::SanitizeFloat(CurrentDamage));

		CurrentDamage = CurrentDamage * Attacker->CurrentSelectedAttack.BasePower;
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / Damage multiplied by attack base power: %s"), *FString::SanitizeFloat(CurrentDamage));

		CurrentDamage = CurrentDamage * FMath::DivideAndRoundUp(Attacker->AvatarData.BattleStats.Attack, TargetAsCharacter->AvatarData.BattleStats.Defence);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / Attackers' divided by defenders' stats: %s"), *FString::SanitizeFloat(FMath::DivideAndRoundUp(Attacker->AvatarData.BattleStats.Attack, TargetAsCharacter->AvatarData.BattleStats.Defence)));
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / Damage multiplied with attackers' divided by defenders' stats: %s"), *FString::SanitizeFloat(CurrentDamage));

		CurrentDamage = FMath::CeilToFloat(CurrentDamage / 50);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / Damage divided by 50: %s"), *FString::SanitizeFloat(CurrentDamage));

		if (CurrentDamage < 1.f) {
			CurrentDamage += 1.f;
		}

		FMath::FloorToFloat(CurrentDamage);

		// Subtract Health
		AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(Attacker->PlayerControllerReference->PlayerState);
		PlayerStateReference->Server_SubtractHealth_Implementation(TargetAsCharacter, CurrentDamage);
	}

	// Apply move effects after the damage has been dealt
	if (!AttackEffectsLibrary_Reference && AttackEffectsLibrary_Class)
	{
		const FActorSpawnParameters SpawnInfo;
		AttackEffectsLibrary_Reference = GetWorld()->SpawnActor<AActor_AttackEffectsLibrary>(AttackEffectsLibrary_Class, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);
	}

	for (int i = 0; i < AttackData.AttackEffectsOnTarget.Num(); i++) {
		AttackEffectsLibrary_Reference->SwitchOnAttackEffect(AttackData.AttackEffectsOnTarget[i], Attacker, Target);
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / End function"));
}


void AStarmark_GameMode::Server_AvatarBeginTurn_Implementation(int CurrentAvatarTurnIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Begin function"));

	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	for (APlayerController_Battle* Controller : GameStateReference->ReturnAllBattlePlayerControllers()) {
		Controller->CurrentSelectedAvatar = GameStateReference->CurrentlyActingAvatar;
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Set the currently acting avatar: %s"), *GameStateReference->CurrentlyActingAvatar->AvatarData.Nickname);

		if (Cast<APlayerController_Battle>(GameStateReference->CurrentlyActingPlayer) == Controller) {
			// Set first Avatar's controller as the currently acting player
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Set the currently acting player"));

			Controller->IsCurrentlyActingPlayer = true;
			Controller->Client_ShowHideHud(true);

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Yaaaaay"));
		} else {
			Controller->IsCurrentlyActingPlayer = false;
			Controller->Client_ShowHideHud(false);

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Boooooo"));
		}

		Controller->Player_OnAvatarTurnChanged();
		Controller->Client_UpdateAttacksInHud(GameStateReference->CurrentlyActingAvatar);

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / First avatar: %s"), *GameStateReference->DynamicAvatarTurnOrder[0]->AvatarData.Nickname);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Begin new turn for player: %s"), *Controller->PlayerDataStruct.PlayerName);
	}

	// Reset the number of avatars died for each player.
	for (int i = 0; i < GameStateReference->PlayerArray.Num(); i++) {
		Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->NumberOfAvatarsDiedThisTurn = 0;
	}

	// Update all the players' HUDs here and avatar icons in turn order
	GameStateReference->SetTurnOrder();

	// testing this
	GameStateReference->OnRepNotify_DynamicAvatarTurnOrderUpdated();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / End function"));
}


void AStarmark_GameMode::Server_AvatarDefeated_Implementation(ACharacter_Pathfinder* Avatar)
{
	APlayerController_Battle* PlayerControllerReference = nullptr;

	for (APlayerController_Battle* Controller : GameStateReference->ReturnAllBattlePlayerControllers()) {
		for (FAvatar_Struct AvatarData : Controller->PlayerDataStruct.CurrentAvatarTeam) {
			if (AvatarData.BattleUniqueID == Avatar->AvatarData.BattleUniqueID) {
				Controller->PlayerDataStruct.CurrentAvatarTeam.Remove(AvatarData);

				GameStateReference->AvatarTurnOrder.Remove(Avatar);
				GameStateReference->DynamicAvatarTurnOrder.Remove(Avatar);

				PlayerControllerReference = Controller;
				break;
			}
		}
	}

	// Find the tile that avatar was at, and remove the 'Occupied' property.
	TArray<AActor*> FoundGridTileActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTileActors);
	for (int i = 0; i < FoundGridTileActors.Num(); i++) {
		if (FMath::IsNearlyEqual(FoundGridTileActors[i]->GetActorLocation().X, Avatar->GetActorLocation().X, 1.f) && FMath::IsNearlyEqual(FoundGridTileActors[i]->GetActorLocation().Y, Avatar->GetActorLocation().Y, 1.f)) {
			Cast<AActor_GridTile>(FoundGridTileActors[i])->Properties.Remove(E_GridTile_Properties::E_Occupied);
			break;
		}
	}

	if (IsValid(PlayerControllerReference)) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / Remove Avatar %s from Turn Order"), *Avatar->AvatarData.AvatarName);
		Server_AssembleTurnOrderText();

		if (PlayerControllerReference->PlayerDataStruct.CurrentAvatarTeam.Num() <= 0) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AStarmark_GameMode / Server_AvatarDefeated / Player has run out of Avatars")));
			GameStateReference->EndOfBattle_Implementation();
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AStarmark_GameMode / Server_AvatarDefeated / Player Avatars Remaining: %d"), Avatar->PlayerControllerReference->PlayerParty.Num()));

			// Tell the player how many avatars they've lost this turn so they can summon more than one if they have to.
			Cast<AStarmark_PlayerState>(PlayerControllerReference->PlayerState)->NumberOfAvatarsDiedThisTurn++;
			
			// Check for any avatars in reserve.
			if (PlayerControllerReference->PlayerClickMode != E_PlayerCharacter_ClickModes::SelectReserveAvatarToSummon) {
				for (int i = 0; i < PlayerControllerReference->PlayerDataStruct.CurrentAvatarTeam.Num(); i++) {
					const FAvatar_Struct& FoundAvatar = PlayerControllerReference->PlayerDataStruct.CurrentAvatarTeam[i];
					if (FoundAvatar.CurrentHealthPoints > 0 && FoundAvatar.IndexInPlayerLibrary >= 4) {
						PlayerControllerReference->SetPlayerClickMode(E_PlayerCharacter_ClickModes::SelectReserveAvatarToSummon);
					
						break;
					}
				}
			}
		}
	}
	
	Avatar->Destroy();
}


void AStarmark_GameMode::EndOfBattle_Implementation()
{
	// Implemented in Blueprints
}