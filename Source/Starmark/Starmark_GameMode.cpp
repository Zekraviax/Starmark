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
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Starmark_Variables.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"


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


UStarmark_GameInstance* AStarmark_GameMode::GetHostPlayerGameStateInstanceReference()
{
	// To-Do: Figure out how to always get the host player
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
	PlayerControllerReferences.Add(NewController);

	// Set-up for the battle
	OnPlayerPostLogin(NewController);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / End function"));
}


// ------------------------- Battle
// OnPlayerPostLogin isn't called when seamlessly travelling, because the clients aren't disconnected
void AStarmark_GameMode::OnPlayerPostLogin(APlayerController_Battle* NewPlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Begin function"));
	
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

	// When all players have joined, begin running the setupfunctions needed to start the battle
	// To-Do: Add a step here to check if a session even exists before trying to fetch the number of connections
	if (!HostPlayerGameInstanceReference) {
		// This should only be done once per battle
		HostPlayerGameInstanceReference = Cast<UStarmark_GameInstance>(NewPlayerController->GetGameInstance());
	}

	// We need to check if a multiplayer session exists or not here
	// If a session does not exist, then its safe to assume the player is in a singleplayer battle
	if (HostPlayerGameInstanceReference->DoesSessionExist()) {
		ExpectedPlayers = HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPrivateConnections + HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPublicConnections;

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected private connections: %d"), HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPrivateConnections);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected public connections: %d"), HostPlayerGameInstanceReference->GetCurrentSessionSettings()->NumPublicConnections);
	} else {
		ExpectedPlayers = 1;
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Total PlayerControllerReferences in array: %d"), PlayerControllerReferences.Num());
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected players: %d"), ExpectedPlayers);

	// Tell the player to have their data passed from their GameInstance to the server
	// by setting it on the PlayerState
	Cast<AStarmark_PlayerState>(NewPlayerController->PlayerState)->Client_UpdatePlayerData();

	// To-Do: Figure out why the ExpectedPlayers is being overridden to 1
	// Don't call this function unless all PlayerStates have their player data
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Start checking if all players are ready"));
	GetWorld()->GetTimerManager().SetTimer(PlayerReadyCheckTimerHandle, this, &AStarmark_GameMode::GetPreBattleChecks, 1.f, false);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / End function"));
}


void AStarmark_GameMode::GetPreBattleChecks_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Checking if all players have readied up"));
	
	bool AreAllPlayersReady = true;

	// Can't fetch the GameState here, it just crashes
	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	if (GameStateReference) {
		for (int i = 0; i < GameStateReference->PlayerArray.Num(); i++) {
			if (GameStateReference->PlayerArray.IsValidIndex(i)) {
				AStarmark_PlayerState* FoundPlayerState = Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i]);

				if (FoundPlayerState->PreBattleCheck == false) {
					AreAllPlayersReady = false;
				}
			} else {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / GameStateReference at index %d is not valid"), i);
			}
		}
	} else {
		AreAllPlayersReady = false;
	}

	if (AreAllPlayersReady) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Checking whether or not all players have traveled. Expected players: %d"), ExpectedPlayers);
		if (ExpectedPlayers >= 2) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Starting a multiplayer battle"));

			Server_BeginMultiplayerBattle();
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / GetPreBattleChecks / Starting a single player battle"));

			TArray<AActor*> FoundPlayerControllers;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_Battle::StaticClass(), FoundPlayerControllers);

			Server_SinglePlayerBeginMultiplayerBattle(Cast<APlayerController_Battle>(FoundPlayerControllers[0]));
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

	for (int i = 0; i < GameStateReference->PlayerArray.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / PlayerState has data with ProfileName: %s"), *Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->PlayerDataStruct.ProfileName);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / PlayerState has %d avatars"), Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->PlayerDataStruct.CurrentAvatarTeam.Num());

		FPlayer_Data PlayerData = Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->PlayerDataStruct;
		PlayerControllerReferences[i]->PlayerDataStruct = PlayerData;
		TArray<FAvatar_Struct> CurrentPlayerTeam = PlayerData.CurrentAvatarTeam;

		int SpawnedAvatarCount = 0;

		for (int j = CurrentPlayerTeam.Num() - 1; j >= 0; j--) {
			if (CurrentPlayerTeam.IsValidIndex(j)) {
				if (CurrentPlayerTeam[j].AvatarName != "Default") {
					if (SpawnedAvatarCount < 4) {
						Server_SpawnAvatar(PlayerControllerReferences[i], (j + 1), CurrentPlayerTeam[j]);
						UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Spawn avatar %s for player %s"), *CurrentPlayerTeam[j].AvatarName, *PlayerControllerReferences[i]->PlayerName);
						
						SpawnedAvatarCount++;
					}
				} else {
					Cast<UStarmark_GameInstance>(PlayerControllerReferences[i]->GetGameInstance())->CurrentProfileReference->CurrentAvatarTeam.RemoveAt(j);
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Remove invalid member %d from PlayerState_PlayerParty"), j);
				}
			}
		}
	}

	Server_MultiplayerBattleCheckAllPlayersReady();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / End function"));
}


void AStarmark_GameMode::Server_SinglePlayerBeginMultiplayerBattle_Implementation(APlayerController_Battle* PlayerControllerReference)
{
	// To-Do: rename this function to something that makes more sense
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SinglePlayerBeginMultiplayerBattle / Begin function"));

	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SinglePlayerBeginMultiplayerBattle / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	for (int i = 0; i < GameStateReference->PlayerArray.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SinglePlayerBeginMultiplayerBattle / PlayerState has data with ProfileName: %s"), *Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->PlayerDataStruct.ProfileName);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SinglePlayerBeginMultiplayerBattle / PlayerState has %d avatars"), Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->PlayerDataStruct.CurrentAvatarTeam.Num());

		FPlayer_Data PlayerData = Cast<AStarmark_PlayerState>(GameStateReference->PlayerArray[i])->PlayerDataStruct;
		PlayerControllerReference->PlayerDataStruct = PlayerData;
		TArray<FAvatar_Struct> CurrentPlayerTeam = PlayerData.CurrentAvatarTeam;

		int SpawnedAvatarCount = 0;

		for (int j = 0; j < CurrentPlayerTeam.Num(); j++) {
			if (CurrentPlayerTeam.IsValidIndex(j)) {
				if (CurrentPlayerTeam[j].AvatarName != "Default") {
					if (SpawnedAvatarCount < 4) {
						Server_SpawnAvatar(PlayerControllerReference, (SpawnedAvatarCount + 1), CurrentPlayerTeam[j]);
						UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SinglePlayerBeginMultiplayerBattle / Spawn avatar %s for player %s"), *CurrentPlayerTeam[j].AvatarName, *PlayerControllerReference->PlayerName);

						SpawnedAvatarCount++;
					}
				} else {
					//Cast<UStarmark_GameInstance>(PlayerControllerReferences[i]->GetGameInstance())->CurrentProfileReference->CurrentAvatarTeam.RemoveAt(j);
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SinglePlayerBeginMultiplayerBattle / Remove invalid member %d from PlayerState_PlayerParty"), j);
				}
			}
		}
	}

	Server_MultiplayerBattleCheckAllPlayersReady();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SinglePlayerBeginMultiplayerBattle / End function"));
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

		// Only set the turn order once all entities are spawned
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
	}


	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / End normal functionality. Begin dev tests..."));

	// Currently, this function only sets an avatar's base stats to their minimum, since there is currently no way for them to scale upwards
	if (GetHostPlayerGameStateInstanceReference()->GetDevSettingsStruct().RecalculateAvatarStatsAtStartOfBattle) {
		TArray<AActor*> Avatars;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), Avatars);
		FString ContextString;

		for (int i = 0; i < Avatars.Num(); i++) {
			ACharacter_Pathfinder* FoundActor = Cast<ACharacter_Pathfinder>(Avatars[i]);
			for (auto Row : GetHostPlayerGameStateInstanceReference()->GetAvatarsDataTable()->GetRowMap()) {
				FAvatar_Struct* AvatarData = (FAvatar_Struct*)(Row.Value);

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

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady / End function"));
}


void AStarmark_GameMode::Server_AssembleTurnOrderText_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Assembling text"));
	
	// The AssembleTurnOrderText should only handle the text, and not images in the hud
	// But whenever the text is updated, shouldn't the images be update to match?
	FString NewTurnOrderText;

	if (!GameStateReference) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / GameStateReference is not valid. Attempting to fetch now."));
		SetGameStateLocalReference();
	}

	for (int i = 0; i < GameStateReference->AvatarTurnOrder.Num(); i++) {
		if (GameStateReference->AvatarTurnOrder[i]->PlayerControllerReference->IsValidLowLevel()) {
			AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GameStateReference->AvatarTurnOrder[i]->PlayerControllerReference->PlayerState);

			// To-Do: Differentiate between player controlled entities and enemy entities in the turn order list.
			NewTurnOrderText.Append(GameStateReference->AvatarTurnOrder[i]->AvatarData.Nickname + "\n");
		} else {
			NewTurnOrderText.Append(GameStateReference->AvatarTurnOrder[i]->AvatarData.Nickname + " / ?\n");
		}
	}

	GameStateReference->CurrentTurnOrderText = NewTurnOrderText;

	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
		PlayerControllerReferences[i]->Local_GetTurnOrderText(GameStateReference->CurrentTurnOrderText);
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Full turn order text: %s"), *NewTurnOrderText);
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Turn order text assembled"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / End function"));
}


void AStarmark_GameMode::Server_SpawnAvatar_Implementation(APlayerController_Battle* PlayerController, int IndexInPlayerParty, FAvatar_Struct AvatarData)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Begin function"));
	
	FString ContextString;
	const FActorSpawnParameters SpawnInfo;
	TArray<AActor*> FoundGridTileActors, ValidMultiplayerSpawnTiles, FoundAvatars;
	TArray<FName> AvatarRowNames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTileActors);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundAvatars);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Attempting to spawn avatar actor"));
	// Find all tiles that can spawn avatars in multiplayer battles
	for (int i = 0; i < FoundGridTileActors.Num(); i++) {
		const AActor_GridTile* GridTileReference = Cast<AActor_GridTile>(FoundGridTileActors[i]);

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Tile AvatarSlotNumber: %d. AssignedMultiplayerUniqueID: %d"), GridTileReference->AvatarSlotNumber, GridTileReference->AssignedMultiplayerUniqueID);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Avatar IndexInPlayerParty: %d. PlayerController UniqueID: %d"), IndexInPlayerParty, PlayerController->MultiplayerUniqueID);

		if (GridTileReference->Properties.Contains(E_GridTile_Properties::E_PlayerAvatarSpawn) &&
			GridTileReference->AvatarSlotNumber == IndexInPlayerParty &&
			GridTileReference->AssignedMultiplayerUniqueID == PlayerController->MultiplayerUniqueID) {
			ValidMultiplayerSpawnTiles.Add(FoundGridTileActors[i]);

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Found valid spawn tile at location: x %d, y %d"),
				FMath::RoundToInt(FoundGridTileActors[i]->GetActorLocation().X),
				FMath::RoundToInt(FoundGridTileActors[i]->GetActorLocation().Y));
		}
	}

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
	// without cloning the data for each avatar
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Give the avatar a copy of the player %s's data"), *PlayerController->PlayerDataStruct.ProfileName);

	NewAvatarActor->ControllingPlayerDataCopy = PlayerController->PlayerDataStruct;
	NewAvatarActor->PlayerControllerReference = PlayerController;

	// To-Do: Clean up the MultiPlayer Unique IDs so that there is only one, used everywhere
	NewAvatarActor->MultiplayerControllerUniqueID = PlayerController->MultiplayerUniqueID;
	PlayerController->PlayerDataStruct.CurrentAvatarTeam[IndexInPlayerParty - 1].OwnerMultiplayerUniqueID = PlayerController->MultiplayerUniqueID;
	PlayerController->PlayerDataStruct.CurrentAvatarTeam[IndexInPlayerParty - 1].BattleUniqueID = FoundAvatars.Num() + 1;
	NewAvatarActor->AvatarData = PlayerController->PlayerDataStruct.CurrentAvatarTeam[IndexInPlayerParty - 1];

	// Get attacks
	for (int i = 0; i < AvatarData.CurrentAttacks.Num(); i++) {
		NewAvatarActor->CurrentKnownAttacks.Add(AvatarData.CurrentAttacks[i]);
	}

	// Sent data to Clients
	// To-Do: Consider sending avatar data to the GameState? Maybe because all players will want to see all avatar data
	NewAvatarActor->Client_GetAvatarData(NewAvatarActor->AvatarData);

	// To-Do: Check if this is redundant, considering it's set in the MultiplayerBeginBattle function
	// Verifying that this is redundant
	// This isn't redundant??
	PlayerController->CurrentSelectedAvatar = NewAvatarActor;

	// Set spawn tile to be occupied
	// ---- Can be refactored into a function ---- //
	if (Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Contains(E_GridTile_Properties::E_None)) {
		Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Remove(E_GridTile_Properties::E_None);
	}

	Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Add(E_GridTile_Properties::E_Occupied);
	Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->OccupyingActor = NewAvatarActor;
	// ---- End refactorable code chunk ---- //

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Avatar spawned"));

	PlayerController->OnRepNotify_CurrentSelectedAvatar();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / End function"));
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
	FActorSpawnParameters SpawnInfo;
	FString ContextString, MoveTypeAsString, TargetTypeAsString;
	TArray<FName> ComplexAttackRowNames = AvatarComplexAttacksDataTable->GetRowNames();
	ACharacter_Pathfinder* TargetAsCharacter = Cast<ACharacter_Pathfinder>(Target);
	float CurrentDamage = 1.f;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_LaunchAttack / Attack chosen: %s"), *AttackName);
	
	for (int i = 0; i < ComplexAttackRowNames.Num(); i++) {
		if (AvatarComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttackRowNames[i], ContextString)->Name == AttackName) {
			AttackData = *AvatarComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttackRowNames[i], ContextString);
			break;
		}
	}

	if (Attacker->CurrentSelectedAttack.AttackCategory == EBattle_AttackCategories::Offensive && Attacker->CurrentSelectedAttack.BasePower > 0) {
		// Calculate damage, one step at a time
		CurrentDamage = FMath::RandRange(1.f, 5.f);
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

	// To-do: Subtract Mana
	//Attacker->AvatarData.CurrentManaPoints -= Attacker->CurrentSelectedAttack.ManaCost;

	// Apply move effects after the damage has been dealt
	if (!AttackEffectsLibrary_Reference && AttackEffectsLibrary_Class) {
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

	if (IsValid(PlayerControllerReference)) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AvatarDefeated / Remove Avatar %s from Turn Order"), *Avatar->AvatarData.AvatarName);
		Server_AssembleTurnOrderText();

		if (PlayerControllerReference->PlayerDataStruct.CurrentAvatarTeam.Num() <= 0) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AStarmark_GameMode / Server_AvatarDefeated / Player has run out of Avatars")));
			GameStateReference->EndOfBattle_Implementation();
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("AStarmark_GameMode / Server_AvatarDefeated / Player Avatars Remaining: %d"), Avatar->PlayerControllerReference->PlayerParty.Num()));

			// Check for any avatars in reserve
			bool FoundAvatarInReserve = false;

			for (int i = 0; i < PlayerControllerReference->PlayerDataStruct.CurrentAvatarTeam.Num(); i++) {
				FAvatar_Struct FoundAvatar = PlayerControllerReference->PlayerDataStruct.CurrentAvatarTeam[i];
				if (FoundAvatar.CurrentHealthPoints > 0 && FoundAvatar.IndexInPlayerLibrary >= 4) {
					// To-Do: Allow the player to summon an Avatar from reserve as a special action here.
					FoundAvatarInReserve = true;

					PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.AttackEffectsOnTarget.Empty();
					PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.AttackEffectsOnTarget.Add(EBattle_AttackEffects::SummonAvatar);
					PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.AttackTargetsInRange = EBattle_AttackTargetsInRange::SelectAllGridTiles;
					PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.AttackPattern = EBattle_AttackPatterns::SingleTile;

					break;
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