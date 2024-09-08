#include "Starmark_GameMode.h"

#include "Actor_AttackEffectsLibrary.h"
#include "Actor_GridTile.h"
#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "PlayerController_Battle.h"
#include "PlayerPawn_Flying.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameMode_Lobby.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Starmark_Variables.h"
#include "Widget_HUD_Battle.h"
#include "Widget_MultiplayerEndMenu.h"


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


AStarmark_GameState* AStarmark_GameMode::GetGameState()
{
	if (!GameStateReference) {
		SetGameStateLocalReference();
	}
	
	return GameStateReference;
}


UStarmark_GameInstance* AStarmark_GameMode::GetHostPlayerGameStateInstanceReference() const
{
	// To-Do: Figure out how to always get the host player
	// (Each player instance can keep track of whether they're the host?)
	//if (!HostPlayerGameInstanceReference) {}

	return HostPlayerGameInstanceReference;
}

TArray<APlayerController_Battle*> AStarmark_GameMode::GetAllBattlePlayerControllers() const
{
	TArray<APlayerController_Battle*> ReturnArray;
	TArray<AActor*> PlayerControllerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_Battle::StaticClass(), PlayerControllerActors);

	for (int i = 0; i < PlayerControllerActors.Num(); i++) {
		if (Cast<APlayerController_Battle>(PlayerControllerActors[i])) {
			ReturnArray.Add(Cast<APlayerController_Battle>(PlayerControllerActors[i]));
		}
	}

	return ReturnArray;
}


TArray<ACharacter_Pathfinder*> AStarmark_GameMode::GetAllAvatars() const
{
	TArray<ACharacter_Pathfinder*> ReturnArray;
	TArray<AActor*> AvatarActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), AvatarActors);

	for (int i = 0; i < AvatarActors.Num(); i++) {
		if (Cast<ACharacter_Pathfinder>(AvatarActors[i])) {
			ReturnArray.Add(Cast<ACharacter_Pathfinder>(AvatarActors[i]));
		}
	}


	return ReturnArray;
}


void AStarmark_GameMode::ServerDumpMultiplayerBattleToLogs_Implementation()
{
	FString DumpToLogsJson;
	FString SingleObjectJson;
	// Get all players and avatars and add their data to the FString
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), ActorList);
	for (int i = 0; i < ActorList.Num(); i++) {
		FJsonObjectConverter::UStructToJsonObjectString(Cast<ACharacter_Pathfinder>(ActorList[i])->AvatarData, SingleObjectJson, 0, 0);
		DumpToLogsJson.Append(SingleObjectJson);
		DumpToLogsJson.Append("\nbattleUniqueId: %d" + FString::FromInt(Cast<ACharacter_Pathfinder>(ActorList[i])->AvatarBattleUniqueID));
		UE_LOG(LogTemp, Warning, TEXT("Append Avatar data to log as FString: %s"), *SingleObjectJson);
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_Battle::StaticClass(), ActorList);
	for (int i = 0; i < ActorList.Num(); i++) {
		FJsonObjectConverter::UStructToJsonObjectString(Cast<APlayerController_Battle>(ActorList[i])->PlayerDataStruct, SingleObjectJson, 0, 0);
		DumpToLogsJson.Append(SingleObjectJson);
		UE_LOG(LogTemp, Warning, TEXT("Append Player data to log as FString: %s"), *SingleObjectJson);
	}
}


// ------------------------- Battle
void AStarmark_GameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / Player has finished loading."));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / AController type: %s."), *C->GetName());
	APlayerController_Battle* NewController = Cast<APlayerController_Battle>(SpawnPlayerController(ROLE_Authority, FVector(0, 0, 0), FRotator(0, 0, 0)));

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / Attempting to swap player controllers."));
	SwapPlayerControllers(Cast<APlayerController>(C), NewController);

	// Set-up for the battle
	PlayerControllerReferences.AddUnique(NewController);
	OnPlayerPostLogin(NewController);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / End function"));
}


// ------------------------- Battle
// OnPlayerPostLogin isn't called when seamlessly travelling, because the clients aren't disconnected
void AStarmark_GameMode::OnPlayerPostLogin(APlayerController_Battle* NewPlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Begin function"));
	PlayerControllerReferences.AddUnique(NewPlayerController);

	// We're assigning each player a unique number just in case two or more players have the same player name.
	NewPlayerController->MultiplayerUniqueID = MultiplayerUniqueIDCounter;
	Cast<AStarmark_PlayerState>(NewPlayerController->PlayerState)->ReplicatedMultiplayerUniqueID = MultiplayerUniqueIDCounter;
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / MultiplayerUniqueIDCounter is: %d."), MultiplayerUniqueIDCounter);
	
	// The one true method of sending the players' data to the server.
	NewPlayerController->ClientSendDataToServer(BattleUniqueIDCounter, MultiplayerUniqueIDCounter);
	BattleUniqueIDCounter += (NewPlayerController->PlayerDataStruct.CurrentAvatarTeam.Num() + 1);
	MultiplayerUniqueIDCounter++;
	
	// Spawn and possess the player pawn.
	TArray<AActor*> FoundPlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundPlayerStartActors);
	FVector Location;
	FRotator Rotation;
	FActorSpawnParameters SpawnInfo;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Search for valid spawn points for the player pawn."));
	if (FoundPlayerStartActors.IsValidIndex(0)) {
		if (FoundPlayerStartActors[0]->IsValidLowLevel()) {
			Location = FoundPlayerStartActors[0]->GetActorLocation();
			Rotation = FoundPlayerStartActors[0]->GetActorRotation();

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Found a location for the player pawn to spawn."));
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / FoundPlayerStartActors[0] is not valid."));
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / FoundPlayerStartActors.IsValidIndex(0) is not valid."));
	}

	NewPlayerController->Possess(GetWorld()->SpawnActor<APlayerPawn_Flying>(PlayerPawnBlueprintClass, Location, Rotation, SpawnInfo));
	NewPlayerController->ClientSetRotation(Rotation, true);
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Player controller possessed pawn: %s."), *NewPlayerController->GetPawn()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Player pawn rotation: %s."), *Rotation.ToString());
	
	// Clear the Server Lobby widget from the players' display.
	NewPlayerController->Client_ClearLobbyFromScreen();

	// Try and figure out which player is hosting the multiplayer battle.
	if (!HostPlayerGameInstanceReference) {
		// This should only be done once per battle.
		HostPlayerGameInstanceReference = Cast<UStarmark_GameInstance>(NewPlayerController->GetGameInstance());
	}

	// Here is where we need to check if a multiplayer session exists.
	// If a session does not exist, then it is safe to assume the player is in a singleplayer battle.
	if (HostPlayerGameInstanceReference->DoesSessionExist()) {
		ExpectedPlayers = HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPrivateConnections + HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPublicConnections;

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected private connections: %d."), HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPrivateConnections);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected public connections: %d."), HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPublicConnections);
	} else {
		ExpectedPlayers = 1;
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Total PlayerControllerReferences in array: %d."), PlayerControllerReferences.Num());
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected players: %d."), ExpectedPlayers);

	// When all players have joined, begin running the setup functions needed to start the battle.
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Start checking if all players are ready."));
	GetWorld()->GetTimerManager().SetTimer(PlayerReadyCheckTimerHandle, this, &AStarmark_GameMode::GetPreBattleChecks, 1.f, false);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / End function."));
}


void AStarmark_GameMode::GetPreBattleChecks_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Checking if all players have readied up."));
	
	bool AreAllPlayersReady = true;
	
	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	if (GameStateReference) {
		for (int i = 0; i < GameStateReference->PlayerArray.Num(); i++) {
			if (GameStateReference->PlayerArray.IsValidIndex(i)) {
				AStarmark_PlayerState* FoundPlayerState = Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i]);
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Player %s has the ready status: %s"), *GetGameState()->PlayerDataStructsArray[i].PlayerName, FoundPlayerState->PreBattleCheck ? TEXT("true") : TEXT("false"));

				if (FoundPlayerState->PreBattleCheck == false) {
					AreAllPlayersReady = false;
					break;
				} else {
					// When the server has received the true check from the player, add their data to the PlayerData Map.
					GetGameState()->PlayerDataAndUniqueIDMap.Add(Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->ReplicatedMultiplayerUniqueID, GetGameState()->PlayerDataStructsArray[i]);
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Player has the index %d and the profile name: %s."), Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->ReplicatedMultiplayerUniqueID, *GetGameState()->PlayerDataStructsArray[i].ProfileName);
				}
			} else {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / GameStateReference at index %d is not valid."), i);
			}
		}
	} else {
		AreAllPlayersReady = false;
	}
	
	if (AreAllPlayersReady) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Checking whether or not all players have traveled. Expected players: %d."), ExpectedPlayers);
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

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / End function."));
}


void AStarmark_GameMode::Server_BeginMultiplayerBattle_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Begin function"));

	for (int i = 0; i < GetGameState()->PlayerDataStructsArray.Num(); i++) {
		// UStructs are value types. If you want to pass-by-reference, use the parent.
		FPlayer_Data& PlayerData = GetGameState()->PassByReferencePlayerDataUsingMultiplayerUniqueID(i);
		TArray<FAvatar_Struct>& CurrentPlayerTeam = PlayerData.CurrentAvatarTeam;
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Double checking player name: %s."), *PlayerData.PlayerName);

		int SpawnedAvatarCount = 0;
		for (int j = 0; j < CurrentPlayerTeam.Num(); j++) {
			if (CurrentPlayerTeam.IsValidIndex(j)) {
				if (CurrentPlayerTeam[j].AvatarName != "Default") {
					if (SpawnedAvatarCount < 4) {
						UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Spawn avatar %s for player %s with UniqueID %d."), *CurrentPlayerTeam[j].AvatarName, *PlayerData.PlayerName, CurrentPlayerTeam[j].BattleUniqueID);

						Server_SpawnAvatar(PlayerControllerReferences[i], PlayerData, (j + 1), CurrentPlayerTeam[j]);
						SpawnedAvatarCount++;
					} else {
						// Calculate variables like Current Health for reserve avatars here.
						UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Avatar %s with UniqueID %d for player %s is in reserve."), *CurrentPlayerTeam[j].AvatarName, BattleUniqueIDCounter, *PlayerData.PlayerName);
					}
				} else {
					GameStateReference->PlayerDataStructsArray[i].CurrentAvatarTeam.RemoveAt(j);
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Invalid Avatar team member at index: %d."), j);
				}
			}
		}
	}

	// Only set the turn order once all entities are spawned.
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Fill the DynamicAvatarTurnOrder array for the first time."));
	GameStateReference->SetTurnOrder();
	GameStateReference->DynamicAvatarTurnOrder = GameStateReference->AvatarTurnOrder;
	GameStateReference->OnRepNotify_DynamicAvatarTurnOrderUpdated();

	// We're going to tell each player which avatar is going first here, after all avatar actors have been spawned.
	TArray<APlayerController_Battle*> PlayerControllerActors = GetAllBattlePlayerControllers();
	for (APlayerController_Battle* Controller : PlayerControllerActors) {
		Controller->CurrentSelectedAvatar = GameStateReference->ReturnCurrentlyActingAvatar();
		Controller->OnRepNotify_CurrentSelectedAvatar();

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Player %s now has Current Acting Avatar %s."), *Controller->PlayerDataStruct.PlayerName, *GameStateReference->ReturnCurrentlyActingAvatar()->AvatarData.Nickname);
	}
	
	Server_MultiplayerBattleCheckAllPlayersReady();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / End function"));
}


void AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Check all %d playerss are ready for the battle."), PlayerControllerReferences.Num());
	
	TArray<bool> ReadyStatuses;
	TArray<AActor*> GridTilesArray;
	TArray<APlayerController_Battle*> PlayerControllerActors = GetAllBattlePlayerControllers();

	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}
	
	for (APlayerController_Battle* Controller : PlayerControllerActors) {
		if (Controller) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Is %s ready to start the battle? %s."), *Controller->PlayerDataStruct.PlayerName, Controller->IsReadyToStartMultiplayerBattle ? TEXT("true") : TEXT("false"));

			ReadyStatuses.Add(Controller->IsReadyToStartMultiplayerBattle);
		}
	}

	if (ReadyStatuses.Contains(false) || ReadyStatuses.Num() < ExpectedPlayers) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Can't assemble turn order text because not all players are ready."));
		GetWorld()->GetTimerManager().SetTimer(PlayerReadyCheckTimerHandle, this, &AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady, 1.f, false);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / All players are ready to start."));
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Assemble the turn order text."));
		Server_AssembleTurnOrderText();

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Update each players' HUD with the currently acting avatars attacks."));
		for (APlayerController_Battle* Controller : PlayerControllerActors) {
			for (int j = 0; j < GameStateReference->AvatarTurnOrder.Num(); j++) {
				if (Controller->MultiplayerUniqueID == GameStateReference->AvatarTurnOrder[j]->MultiplayerControllerUniqueID) {
					Controller->Client_UpdateAttacksInHud(GameStateReference->AvatarTurnOrder[j]);
					break;
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Update all the other HUD elements for player %s."), *Controller->PlayerDataStruct.PlayerName);
			Controller->Client_UpdateCurrentAvatarInHud(GameStateReference->CurrentlyActingAvatar);
			Controller->Client_UpdateCurrentTurnOrderInHud(GameStateReference->DynamicAvatarTurnOrderImages);
		}

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Update all avatar decals."));
		Server_UpdateAllAvatarDecals();
		
		// Set first Avatar's controller as the currently acting player.
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / Set the currently acting player: %s."), *GameStateReference->ReturnCurrentlyActingPlayer()->PlayerName);

		// Clear Combat Log
		GetGameState()->AppendStringToCombatLog("Battle begin!");
		
		// Show the currently acting players' HUD, and hide all other players' HUDs.
		GameStateReference->ShowHideAllPlayerHuds();


		
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / End normal functionality. Begin dev tests..."));

		// Currently, this function only sets an avatar's base stats to their minimum, since there is currently no way for them to scale upwards.
		if (GetHostPlayerGameStateInstanceReference()->GetDevSettingsStruct().RecalculateAvatarStatsAtStartOfBattle) {
			TArray<ACharacter_Pathfinder*> Avatars = GetAllAvatars();
			FString ContextString;

			for (int i = 0; i < Avatars.Num(); i++) {
				for (const auto Row : GetHostPlayerGameStateInstanceReference()->GetAvatarsDataTable()->GetRowMap()) {
					const FAvatar_Struct* AvatarData = reinterpret_cast<FAvatar_Struct*>(Row.Value);

					if (AvatarData->AvatarName == Avatars[i]->AvatarData.AvatarName) {
						Avatars[i]->AvatarData.BattleStats = AvatarData->SpeciesMinimumStats;
						Avatars[i]->AvatarData.CurrentHealthPoints = AvatarData->SpeciesMinimumStats.MaximumHealthPoints;
						Avatars[i]->AvatarData.CurrentManaPoints = AvatarData->SpeciesMinimumStats.MaximumManaPoints;
						break;
					}
				}
			}
		}
		
		for (APlayerController_Battle* Controller : PlayerControllerActors) {
			Controller->SetBattleWidgetVariables();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / End function"));
}


void AStarmark_GameMode::Server_AssembleTurnOrderText_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Assembling text."));
	
	// The AssembleTurnOrderText should only handle the text, and not images in the HUD.
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

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Turn order text assembled."));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / End function."));
}


void AStarmark_GameMode::Server_SpawnAvatar_Implementation(APlayerController_Battle* PlayerController, FPlayer_Data PlayerData, int IndexInPlayerParty, FAvatar_Struct AvatarData, FVector Location = FVector(0, 0, -9999))
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Begin function"));
	
	FString ContextString;
	TArray<AActor*> FoundGridTileActors, ValidMultiplayerSpawnTiles, FoundAvatars;
	TArray<FName> AvatarRowNames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTileActors);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundAvatars);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Attempting to spawn avatar actor."));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Avatar's IndexInPlayerParty: %d."), IndexInPlayerParty);
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Player's MultiplayerUniqueID: %d."), PlayerController->MultiplayerUniqueID);
	
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

		// Avatar Stats.
		NewAvatarActor->AvatarData.CurrentHealthPoints = NewAvatarActor->AvatarData.BattleStats.MaximumHealthPoints;
		NewAvatarActor->AvatarData.CurrentManaPoints = NewAvatarActor->AvatarData.BattleStats.MaximumManaPoints;
		NewAvatarActor->AvatarData.CurrentTileMoves = NewAvatarActor->AvatarData.MaximumTileMoves;

		// To-Do: Figure out a way for each avatar to have a reference to their players' data
		// without cloning the data for each avatar.
		// (pass-by-reference)
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Give the avatar a copy of the Player %s's data"), *PlayerData.PlayerName);
		NewAvatarActor->PlayerControllerReference = PlayerController;

		// To-Do: Clean up all of this. (pass values by-reference)
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Received a copy of the Avatar's data with the UniqueID %d"), AvatarData.BattleUniqueID);
		NewAvatarActor->AvatarBattleUniqueID = AvatarData.BattleUniqueID;
		NewAvatarActor->MultiplayerControllerUniqueID = PlayerController->MultiplayerUniqueID;
		NewAvatarActor->AvatarData = AvatarData;

		// Get attacks
		UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(GetGameInstance());
		for (int i = 0; i < AvatarData.CurrentEquippedAttackNames.Num(); i++) {
			if (AvatarData.CurrentEquippedAttackNames[i] != FName("None")) {
				NewAvatarActor->CurrentKnownAttacks.Add(*GameInstanceReference->ReturnAttackFromDataTable(AvatarData.CurrentEquippedAttackNames[i]));
			}
		}

		// Sent data to the actor.
		// To-Do: Consider sending avatar data to the GameState? Because all players will want to see all avatar data.
		NewAvatarActor->Client_GetAvatarData(NewAvatarActor->AvatarData);
		//PlayerController->CurrentSelectedAvatar = NewAvatarActor;

		// Set spawn tile to be occupied.
		// ---- Can be refactored into a function ---- //
		if (Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Contains(E_GridTile_Properties::E_None)) {
			Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Remove(E_GridTile_Properties::E_None);
		}

		Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Add(E_GridTile_Properties::E_Occupied);
		Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->OccupyingActor = NewAvatarActor;
		// ---- End refactorable code chunk ---- //

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Avatar spawned."));
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Failed to find valid tile for Avatar at index: %d"), IndexInPlayerParty);
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / End function."));
}


void AStarmark_GameMode::Server_UpdateAllAvatarDecals_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_UpdateAllAvatarDecals / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_UpdateAllAvatarDecals / Updating all avatar decals."));
	
	TArray<AActor*> Avatars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), Avatars);
	ACharacter_Pathfinder* CurrentlyActingAvatar;

	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	// This chunk of code assigns the Currently Acting Avatar.
	// It should be refactored into its own function.
	/*if (GameStateReference->AvatarTurnOrder.IsValidIndex(GameStateReference->CurrentAvatarTurnIndex)) {
		CurrentlyActingAvatar = GameStateReference->AvatarTurnOrder[GameStateReference->CurrentAvatarTurnIndex];
	} else { 
		CurrentlyActingAvatar = GameStateReference->AvatarTurnOrder[0];
	}*/
	if (!GameStateReference->DynamicAvatarTurnOrder.IsValidIndex(0)) {
		GameStateReference->DynamicAvatarTurnOrder = GameStateReference->AvatarTurnOrder;
		GameStateReference->OnRepNotify_DynamicAvatarTurnOrderUpdated();
	}
	
	CurrentlyActingAvatar = GameStateReference->DynamicAvatarTurnOrder[0];

	for (int i = 0; i < Avatars.Num(); i++) {
		ACharacter_Pathfinder* FoundActor = Cast<ACharacter_Pathfinder>(Avatars[i]);

		if (IsValid(FoundActor->PlayerControllerReference)) {
			bool IsCurrentlyActing = false;

			if (CurrentlyActingAvatar == Avatars[i]) {
				IsCurrentlyActing = true;
			}

			//FoundActor->MultiplayerControllerUniqueID = FoundActor->PlayerControllerReference->MultiplayerUniqueID;

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

	GetGameState()->AppendStringToCombatLog(Attacker->PlayerControllerReference->PlayerDataStruct.PlayerName + "'s " + Attacker->AvatarData.Nickname + " used " +
		AttackName + " on " + TargetAsCharacter->PlayerControllerReference->PlayerDataStruct.PlayerName + "'s " + TargetAsCharacter->AvatarData.Nickname + "!");

	// Damage calculation
	if (Attacker->CurrentSelectedAttack.AttackCategory == EBattle_AttackCategories::Offensive && Attacker->CurrentSelectedAttack.BasePower > 0) {
		// Calculate damage, one step at a time.
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
		
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / Target health: %d"), TargetAsCharacter->AvatarData.CurrentHealthPoints);
		TargetAsCharacter->AvatarData.CurrentHealthPoints -= CurrentDamage;
		GetGameState()->AppendStringToCombatLog(TargetAsCharacter->PlayerControllerReference->PlayerDataStruct.PlayerName + "'s " + TargetAsCharacter->AvatarData.Nickname + " took "
			+ FString::FromInt(CurrentDamage) + " damage!");

		if (TargetAsCharacter->AvatarData.CurrentHealthPoints <= 0) {
			// If the avatar was defeated, remove it from the turn order.
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / The avatar %s was defeated."), *TargetAsCharacter->AvatarData.Nickname);
			GetGameState()->AppendStringToCombatLog(TargetAsCharacter->PlayerControllerReference->PlayerDataStruct.PlayerName + "'s " + TargetAsCharacter->AvatarData.Nickname + " has been defeated!");
			Server_AvatarDefeated(TargetAsCharacter);
		}
	}

	// Apply some move effects after the damage has been dealt.
	if (!AttackEffectsLibrary_Reference && AttackEffectsLibrary_Class) {
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
	// List of things that need to happen when an Avatar starts their turn (excluding pre-turn start checks):
	//
	// Each player needs to know which Avatar is currently acting.
	// Each player needs to know whether they are acting this turn.
	// The acting players' HUD needs to be shown to them, and all other players' HUDs need to be hidden.
	// Reset the number of Avatars that died in a turn for each player.
	//
	
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Begin function."));

	// Use this to break out of the function if a player needs to summon a reserve avatar.
	bool PlayerFailedPreStartOfTurnCheck = false;
	TArray<AActor*> AvatarActors;
	TArray<APlayerController_Battle*> ControllerActorsArray = GetAllBattlePlayerControllers();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), AvatarActors);

	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	// Pre Start-Of-Turn Checks:
	// Check if any players still need to summon avatars to replace those that have died.
	// Make sure that all avatars are in the normal AvatarTurnOrder array so that the Dynamic Turn Order can be calculated.
	// If at least one player needs to summon an avatar, stop the start-of-turn checks and have them summon an avatar.
	// Then repeat the process until nobody can or needs to summon avatars.
	for (APlayerController_Battle* Controller : ControllerActorsArray) {
		if (Controller) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Pre start-of-turn check for player %s."), *Controller->PlayerDataStruct.PlayerName);

			AStarmark_PlayerState* PlayerState = Cast<AStarmark_PlayerState>(Controller->PlayerState);
			FPlayer_Data LocalPlayerReference = GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(PlayerState->ReplicatedMultiplayerUniqueID);
			int DeployedAvatarCount = 0, TotalAvatarsInParty = GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(PlayerState->ReplicatedMultiplayerUniqueID).CurrentAvatarTeam.Num();
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Player with Multiplayer ID %d has %d avatars remaining in their party."), PlayerState->ReplicatedMultiplayerUniqueID, TotalAvatarsInParty);

			for (AActor* AvatarActor : AvatarActors) {
				ACharacter_Pathfinder* Avatar = Cast<ACharacter_Pathfinder>(AvatarActor);
				if (Avatar) {
					if (Avatar->MultiplayerControllerUniqueID == Controller->MultiplayerUniqueID) {
						DeployedAvatarCount++;
						UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Found 1 avatar belonging to the player %s. Total avatars found so far: %d."), *LocalPlayerReference.PlayerName, DeployedAvatarCount);
					} else {
						UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Found an avatar that doesn't belong to the player %s."), *LocalPlayerReference.PlayerName);
					}
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Player %s has %d deployed avatar(s) and %d avatar(s) in their party."), *LocalPlayerReference.PlayerName, DeployedAvatarCount, TotalAvatarsInParty);
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Player %s has lost %d avatar(s) this turn"), *LocalPlayerReference.PlayerName, PlayerState->NumberOfAvatarsDiedThisTurn);
			
			if (PlayerState->NumberOfAvatarsDiedThisTurn > 0 && TotalAvatarsInParty <= 4 && DeployedAvatarCount >= TotalAvatarsInParty) {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / There's a player that has had more avatars died than they can summon."));
				PlayerState->NumberOfAvatarsDiedThisTurn = 0;

				// Tell the game to re-calculate the turn order.
				GameStateReference->SetTurnOrder();
			} else if (PlayerState->NumberOfAvatarsDiedThisTurn > 0) {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / There's a player that still needs to summon an avatar."));
				
				Controller->SetPlayerClickMode(E_PlayerCharacter_ClickModes::SelectReserveAvatarToSummon);

				// Don't just break out of the loop, break out of the whole function.
				PlayerFailedPreStartOfTurnCheck = true;
				return;
			} else {
				// The player doesn't need to summon any reserve avatars.
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Player %s doesn't need to summon any reserve avatars."), *LocalPlayerReference.PlayerName);
			}
		}
	}
	
	// End Start-Of-Turn Checks:
	// Find the player that should be acting, and update and show their HUD.
	// Hide everyone else's HUD.
	if (!PlayerFailedPreStartOfTurnCheck) {
		if (AvatarDiedThisTurn) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / One or more avatars died this turn, so re-calculate the turn order here."));
			GameStateReference->SetTurnOrder();
			GameStateReference->DynamicAvatarTurnOrder = GameStateReference->AvatarTurnOrder;
		}

		GameStateReference->OnRepNotify_DynamicAvatarTurnOrderUpdated();

		// Reset the currently acting avatars' moves
		GameStateReference->CurrentlyActingAvatar->AvatarData.CurrentTileMoves = GameStateReference->CurrentlyActingAvatar->AvatarData.MaximumTileMoves;
	
		for (APlayerController_Battle* Controller : ControllerActorsArray) {
			if (Controller) {
				Controller->CurrentSelectedAvatar = GameStateReference->ReturnCurrentlyActingAvatar();
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Set the currently acting avatar: %s"), *GameStateReference->CurrentlyActingAvatar->AvatarData.Nickname);

				// Set the first Avatar's controller as the currently acting player.
				if (Controller == GameStateReference->ReturnCurrentlyActingPlayer()) {
					Controller->IsCurrentlyActingPlayer = true;
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Yaaaaay"));
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Set the currently acting player: %s."), *Controller->PlayerDataStruct.PlayerName);
				} else {
					Controller->IsCurrentlyActingPlayer = false;
					Controller->Client_ShowHideHud(false);
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Boooooo"));
				}

				Controller->Player_OnAvatarTurnChanged();
				Controller->Client_UpdateAttacksInHud(GameStateReference->CurrentlyActingAvatar);
				Controller->Client_UpdateCurrentAvatarInHud(GameStateReference->CurrentlyActingAvatar);
				Controller->Client_UpdateCurrentTurnOrderInHud(GameStateReference->DynamicAvatarTurnOrderImages);

				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / First avatar: %s"), *GameStateReference->DynamicAvatarTurnOrder[0]->AvatarData.Nickname);
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / Begin new turn for player: %s"), *Controller->PlayerDataStruct.PlayerName);
			}
		}
		
		// Reset the number of avatars died for each player.
		AvatarDiedThisTurn = false;
		for (int i = 0; i < GameStateReference->PlayerArray.Num(); i++) {
			Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->NumberOfAvatarsDiedThisTurn = 0;
		}
	
		GameStateReference->ShowHideAllPlayerHuds();
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarBeginTurn / End function"));
}


void AStarmark_GameMode::Server_AvatarDefeated_Implementation(ACharacter_Pathfinder* Avatar)
{
	// List of things this function should do (not in any particular order):
	//
	// Remove the Avatar from the players' team in the GameState player data array.
	// Remove the Avatar from the Static TurnOrder array and the Dynamic TurnOrder array.
	// Delete the Avatar actor.
	// Synchronize the players' data to their controllers?
	//

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / Begin function"));

	APlayerController_Battle* PlayerControllerReference = nullptr;
	TArray<AActor*> PlayerControllerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_Battle::StaticClass(), PlayerControllerActors);
	bool ContinueLooping = true;

	// Remove the avatar from the turn order.
	// The turn order will be recalculated a bit after this.
	GameStateReference->AvatarTurnOrder.Remove(Avatar);
	GameStateReference->DynamicAvatarTurnOrder.Remove(Avatar);
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / Remove the Avatar %s from the turn order."), *Avatar->AvatarData.Nickname);
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / AvatarTurnOrder length: %d."), GameStateReference->AvatarTurnOrder.Num());

	// Search for the avatars' controllers' data to remove it from their team.
	for (AActor* ControllerActor : PlayerControllerActors) {
		APlayerController_Battle* Controller = Cast<APlayerController_Battle>(ControllerActor);
		
		if (Controller && ContinueLooping) {
			if (Controller->MultiplayerUniqueID == Avatar->PlayerControllerReference->MultiplayerUniqueID) {
				FPlayer_Data PlayerData = GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(Controller->MultiplayerUniqueID);
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / Searching for Avatar in Player %s's data. Player has ID %d"), *PlayerData.PlayerName, Controller->MultiplayerUniqueID);
				
				for (FAvatar_Struct AvatarData : GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(Controller->MultiplayerUniqueID).CurrentAvatarTeam) {
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / Comparing Actor's UniqueID %d with Avatar-In-Teams' ID %d"), Avatar->AvatarData.BattleUniqueID, AvatarData.BattleUniqueID);
					
					if (AvatarData.BattleUniqueID == Avatar->AvatarData.BattleUniqueID) {
						UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / Before: Player has %d avatars remaining."), GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(Controller->MultiplayerUniqueID).CurrentAvatarTeam.Num());
						GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(Controller->MultiplayerUniqueID).CurrentAvatarTeam.Remove(AvatarData);
						UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / After: Player has %d avatars remaining."), GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(Controller->MultiplayerUniqueID).CurrentAvatarTeam.Num());

						if (GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(Controller->MultiplayerUniqueID).CurrentAvatarTeam.Num() <= 0) {
							// The player has been defeated.
							ContinueLooping = false;
							EndOfBattle();
						}
						
						PlayerControllerReference = Controller;
						break;
					}
				}
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

	// Check if the player has run out of Avatars here.
	if (IsValid(PlayerControllerReference)) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / Remove Avatar %s from Turn Order"), *Avatar->AvatarData.AvatarName);
		Server_AssembleTurnOrderText();

		if (GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(PlayerControllerReference->MultiplayerUniqueID).CurrentAvatarTeam.Num() <= 0) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AStarmark_GameMode / Server_AvatarDefeated / Player has run out of Avatars.")));
			GameStateReference->EndOfBattle_Implementation();
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AStarmark_GameMode / Server_AvatarDefeated / Player avatars remaining: %d."), Avatar->PlayerControllerReference->PlayerParty.Num()));

			// Tell the player how many avatars they've lost this turn so they can summon more than one if they have to.
			Cast<AStarmark_PlayerState>(PlayerControllerReference->PlayerState)->NumberOfAvatarsDiedThisTurn++;
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AStarmark_GameMode / Server_AvatarDefeated / The player has lost %d avatars this turn."), Cast<AStarmark_PlayerState>(PlayerControllerReference->PlayerState)->NumberOfAvatarsDiedThisTurn++));
			
			// Check for any avatars in reserve.
			if (PlayerControllerReference->PlayerClickMode != E_PlayerCharacter_ClickModes::SelectReserveAvatarToSummon) {
				for (int i = 0; i < GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(PlayerControllerReference->MultiplayerUniqueID).CurrentAvatarTeam.Num(); i++) {
					const FAvatar_Struct& FoundAvatar = GetGameState()->FindPlayerDataUsingMultiplayerUniqueID(PlayerControllerReference->MultiplayerUniqueID).CurrentAvatarTeam[i];
					if (FoundAvatar.CurrentHealthPoints > 0 && FoundAvatar.IndexInPlayerLibrary >= 4) {
						PlayerControllerReference->SetPlayerClickMode(E_PlayerCharacter_ClickModes::SelectReserveAvatarToSummon);
					
						break;
					}
				}
			}
		}
	}
	
	// When the turn ends, the dynamic turn order should be updated.
	AvatarDiedThisTurn = true;
	Avatar->Destroy();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / End function"));
}


void AStarmark_GameMode::EndOfBattle() const
{
	TArray<APlayerController_Battle*> PlayerControllerActors = GetAllBattlePlayerControllers();

	for (APlayerController_Battle* Controller : PlayerControllerActors) {
		Controller->CreateEndOfBattleWidget();
	}

	for (APlayerController_Battle* Controller : PlayerControllerActors) {
		int LocalRemainingAvatarsCount = 0;
		
		for (FAvatar_Struct Avatar : Controller->PlayerParty) {
			if (Avatar.CurrentHealthPoints > 0) {
				LocalRemainingAvatarsCount++;
			}
		}

		for (APlayerController_Battle* Controller2 : PlayerControllerActors) {
			Controller2->EndOfBattleWidgetReference->AppendPlayerDataToResultsString(Controller->ReturnPlayerData().PlayerName + ": " +
				FString::FromInt(LocalRemainingAvatarsCount) + "/" + FString::FromInt(Controller->PlayerParty.Num()) + " avatars remaining.");
		}
	}
}
