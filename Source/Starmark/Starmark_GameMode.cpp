#include "Starmark_GameMode.h"

#include "Actor_AttackEffectsLibrary.h"
#include "Actor_GridTile.h"
#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController_Battle.h"
#include "PlayerController_Lobby.h"
#include "PlayerPawn_Flying.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Starmark_Variables.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"


void AStarmark_GameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / Player has finished loading"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / AController type: %s"), *C->GetName());
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / PlayerController has data with ProfileName: %s"), *Cast<APlayerController_Lobby>(C)->PlayerDataStruct.ProfileName);

	APlayerController_Battle* NewController = Cast<APlayerController_Battle>(SpawnPlayerController(ROLE_Authority, FVector(0, 0, 0), FRotator(0, 0, 0)));
	NewController->PlayerDataStruct = Cast<APlayerController_Lobby>(C)->PlayerDataStruct;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / HandleSeamlessTravelPlayer / Attempting to swap player controllers"));
	SwapPlayerControllers(Cast<APlayerController>(C), NewController);

	// Set-up for the new controller
	NewController->CreateBattleWidget();
	PlayerControllerReferences.Add(NewController);

	// Set-up for the battle
	OnPlayerPostLogin(NewController);
}


// ------------------------- Battle
// OnPlayerPostLogin isn't called when seamlessly travelling, because the clients aren't disconnected
void AStarmark_GameMode::OnPlayerPostLogin(APlayerController_Battle* NewPlayerController)
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

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Found a location for the player's pawn to spawn"));
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

	MultiplayerUniqueIDCounter++;
	NewPlayerController->MultiplayerUniqueID = MultiplayerUniqueIDCounter;
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / MultiplayerUniqueIDCounter is: %d"), MultiplayerUniqueIDCounter);

	// Clear Combat Log
	if (CombatLogTextArray.Num() > 0) {
		CombatLogTextArray.Empty();
	}

	// Clear the Server Lobby menu from the players' display
	NewPlayerController->Client_ClearLobbyFromScreen();

	// Retrieve the player's profile from the player state?
	PlayerProfileReferences.Add(Cast<AStarmark_PlayerState>(NewPlayerController->PlayerState)->ReturnPlayerData());

	// When all players have joined, begin running the functions needed to start the battle
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Total PlayerControllerReferences in array: %d"), PlayerControllerReferences.Num());
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Expected players: %d"), ExpectedPlayers);

	// To-Do: Figure out why the ExpectedPlayers is being overridden to 1
	if (PlayerControllerReferences.Num() >= 2) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Call Server_BeginMultiplayerBattle()"));
		Server_BeginMultiplayerBattle();
	}

	/*
	ExpectedPlayers = 2;

	if (ExpectedPlayers >= 2 && PlayerControllerReferences.Num() >= ExpectedPlayers) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Call Server_BeginMultiplayerBattle()"));
		Server_BeginMultiplayerBattle();
	} else if (ExpectedPlayers == 1) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / Begin a single player battle"));
		Server_SinglePlayerBeginMultiplayerBattle(NewPlayerController);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / OnPlayerPostLogin / The battle can't begin because something doesn't add up"));
	}
	*/
}


void AStarmark_GameMode::Server_BeginMultiplayerBattle_Implementation()
{
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	for (int p = 0; p < PlayerControllerReferences.Num(); p++) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Found player profile %s at index: %d"), *(Cast<AStarmark_PlayerState>(PlayerControllerReferences[p]->PlayerState))->ReplicatedPlayerName, p);
	}

	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
		PlayerControllerReferences[i]->Server_GetDataFromProfile();
		TArray<FAvatar_Struct> CurrentPlayerTeam = PlayerControllerReferences[i]->PlayerDataStruct.CurrentAvatarTeam;

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Found player %s, with %d avatar(s)"), *PlayerControllerReferences[i]->PlayerName, CurrentPlayerTeam.Num());

		int SpawnedAvatarCount = 0;

		for (int j = CurrentPlayerTeam.Num() - 1; j >= 0; j--) {
			if (CurrentPlayerTeam[j].AvatarName != "Default") {
				if (SpawnedAvatarCount < 4) {
					Server_SpawnAvatar(PlayerControllerReferences[i], (j + 1), CurrentPlayerTeam[j]);
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Spawn Avatar %s for player %s"), *CurrentPlayerTeam[j].AvatarName, *PlayerControllerReferences[i]->PlayerName);
					SpawnedAvatarCount++;
				}
			} else {
				Cast<UStarmark_GameInstance>(PlayerControllerReferences[i]->GetGameInstance())->CurrentProfileReference->CurrentAvatarTeam.RemoveAt(j);
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_BeginMultiplayerBattle / Remove invalid member %d from PlayerState_PlayerParty"), j);
			}
		}
	}

	Server_MultiplayerBattleCheckAllPlayersReady();
}


void AStarmark_GameMode::Server_SinglePlayerBeginMultiplayerBattle_Implementation(APlayerController_Battle* PlayerControllerReference)
{
	TArray<FAvatar_Struct> CurrentPlayerTeam = Cast<UStarmark_GameInstance>(PlayerControllerReferences[0]->GetGameInstance())->CurrentProfileReference->CurrentAvatarTeam;
	int SpawnedAvatarCount = 0;

	for (int j = CurrentPlayerTeam.Num() - 1; j >= 0; j--) {
		if (CurrentPlayerTeam[j].AvatarName != "Default") {
			if (SpawnedAvatarCount < 4) {
				// Make sure each character can use all of their attacks
				//for (int x = 0; x < CurrentPlayerTeam[j].Attacks.Num(); x++) {
				//	CurrentPlayerTeam[j].CurrentAttacks.Add(*AttacksDataTable->FindRow<FAvatar_AttackStruct>((CurrentPlayerTeam[j].Attacks[x].RowName), GameModeContextString));
				//}

				// Spawn the actor
				Server_SpawnAvatar(PlayerControllerReferences[0], j + 1, CurrentPlayerTeam[j]);
				SpawnedAvatarCount++;
			}
		} else {
			Cast<UStarmark_GameInstance>(PlayerControllerReferences[0]->GetGameInstance())->CurrentProfileReference->CurrentAvatarTeam.RemoveAt(j);
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SinglePlayerBeginMultiplayerBattle / Remove invalid member %d from PlayerState_PlayerParty"), j);
		}
	}

	Server_MultiplayerBattleCheckAllPlayersReady();
}


void AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady_Implementation()
{
	TArray<bool> ReadyStatuses;
	TArray<AActor*> GridTilesArray;
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady_Implementation / Check each PlayerController is get ready for the battle"));
	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
		ReadyStatuses.Add(PlayerControllerReferences[i]->IsReadyToStartMultiplayerBattle);
	}

	// Assemble turn order text
	if (ReadyStatuses.Contains(false) || ReadyStatuses.Num() < ExpectedPlayers) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady_Implementation / Can't assemble turn order text because not all players are ready"));
		GetWorld()->GetTimerManager().SetTimer(PlayerReadyCheckTimerHandle, this, &AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady, 0.5f, false);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady_Implementation / All players are ready to start"));
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady_Implementation / Set the turn order for the avatars"));

		// Only set the turn order once all entities are spawned
		for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
			GameStateReference->SetTurnOrder(PlayerControllerReferences);
		}

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady_Implementation / Assemble the turn order text"));
		Server_AssembleTurnOrderText();

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady_Implementation / Assign UniqueIDs and CurrentSelectedAvatars"));
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady_Implementation / Update each player's attacks in the HUD"));
		
		for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
			for (int j = 0; j < GameStateReference->AvatarTurnOrder.Num(); j++) {
				if (PlayerControllerReferences[i]->MultiplayerUniqueID == GameStateReference->AvatarTurnOrder[j]->MultiplayerControllerUniqueID) {
					PlayerControllerReferences[i]->CurrentSelectedAvatar = GameStateReference->AvatarTurnOrder[j];
					PlayerControllerReferences[i]->Client_UpdateAttacksInHud();
					break;
				}
			}

			PlayerControllerReferences[i]->SetBattleWidgetVariables();
		}

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady_Implementation / Update all avatar decals"));
		Server_UpdateAllAvatarDecals();
		
		// Set first Avatar's controller as the currently acting player
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_MultiplayerBattleCheckAllPlayersReady_Implementation / Set the currently acting player"));
		GameStateReference->AvatarTurnOrder[0]->PlayerControllerReference->IsCurrentlyActingPlayer = true;
		GameStateReference->AvatarTurnOrder[0]->PlayerControllerReference->Client_UpdateAttacksInHud();

		//for (int i = 0; i < PlayerControllerReferences.Num(); i++) {}
	}
}


void AStarmark_GameMode::Server_AssembleTurnOrderText_Implementation()
{
	// The AssembleTurnOrderText should only handle the text, and not images in the hud
	// But whenever the text is updated, shouldn't the images be update to match?
	FString NewTurnOrderText;

	if (GameStateReference == nullptr) {
		GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());
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
		PlayerControllerReferences[i]->Client_GetTurnOrderText(GameStateReference->CurrentTurnOrderText);
		PlayerControllerReferences[i]->Server_GetEntitiesInTurnOrder(GameStateReference->CurrentAvatarTurnIndex);
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_AssembleTurnOrderText / Turn order text assembled"));
}


void AStarmark_GameMode::Server_SpawnAvatar_Implementation(APlayerController_Battle* PlayerController, int IndexInPlayerParty, FAvatar_Struct AvatarData)
{
	FString ContextString;
	const FActorSpawnParameters SpawnInfo;
	TArray<AActor*> FoundGridTileActors, ValidMultiplayerSpawnTiles;
	TArray<FName> AvatarRowNames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTileActors);


	// Find all tiles that can spawn avatars in multiplayer battles
	for (int i = 0; i < FoundGridTileActors.Num(); i++) {
		const AActor_GridTile* GridTileReference = Cast<AActor_GridTile>(FoundGridTileActors[i]);

		if (GridTileReference->Properties.Contains(E_GridTile_Properties::E_PlayerAvatarSpawn) && GridTileReference->AvatarSlotNumber == IndexInPlayerParty) {
			ValidMultiplayerSpawnTiles.Add(FoundGridTileActors[i]);
		}
	}

	FVector Location = ValidMultiplayerSpawnTiles[0]->GetActorLocation();
	Location.Z = 1000;

	ACharacter_Pathfinder* NewAvatarActor = GetWorld()->SpawnActor<ACharacter_Pathfinder>(AvatarBlueprintClass, Location, FRotator::ZeroRotator, SpawnInfo);
	NewAvatarActor->AvatarData = AvatarData;

	// Avatar Stats
	NewAvatarActor->AvatarData.CurrentHealthPoints = NewAvatarActor->AvatarData.BattleStats.MaximumHealthPoints;
	NewAvatarActor->AvatarData.CurrentManaPoints = NewAvatarActor->AvatarData.BattleStats.MaximumManaPoints;
	NewAvatarActor->AvatarData.CurrentTileMoves = NewAvatarActor->AvatarData.MaximumTileMoves;

	// MultiplayerUniqueID
	NewAvatarActor->PlayerControllerReference = PlayerController;
	NewAvatarActor->MultiplayerControllerUniqueID = PlayerController->MultiplayerUniqueID;

	// Get attacks
	for (int i = 0; i < AvatarData.CurrentAttacks.Num(); i++) {
		NewAvatarActor->CurrentKnownAttacks.Add(AvatarData.CurrentAttacks[i]);
	}

	// Sent data to Clients
	NewAvatarActor->Client_GetAvatarData(NewAvatarActor->AvatarData);

	PlayerController->CurrentSelectedAvatar = NewAvatarActor;
	PlayerController->OnRepNotify_CurrentSelectedAvatar();

	// Set spawn tile to be occupied
	if (Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Contains(E_GridTile_Properties::E_None)) {
		Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Remove(E_GridTile_Properties::E_None);
	}

	Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->Properties.Add(E_GridTile_Properties::E_Occupied);
	Cast<AActor_GridTile>(ValidMultiplayerSpawnTiles[0])->OccupyingActor = NewAvatarActor;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Turn order text assembled"));
}


void AStarmark_GameMode::Server_UpdateAllAvatarDecals_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode / Server_SpawnAvatar / Updating all avatar decals"));
	
	TArray<AActor*> Avatars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), Avatars);
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());
	ACharacter_Pathfinder* CurrentlyActingAvatar;

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
}


void AStarmark_GameMode::Server_LaunchAttack_Implementation(ACharacter_Pathfinder* Attacker, AActor* Target, const FString& AttackName)
{
	FAvatar_AttackStruct AttackData;
	FActorSpawnParameters SpawnInfo;
	FString ContextString, MoveTypeAsString, TargetTypeAsString;
	TArray<FName> ComplexAttackRowNames = AvatarComplexAttacksDataTable->GetRowNames();
	ACharacter_Pathfinder* TargetAsCharacter = Cast<ACharacter_Pathfinder>(Target);
	float CurrentDamage = 1.f;

	UE_LOG(LogTemp, Warning, TEXT("Server_LaunchAttack / Attack chosen: %s"), *AttackName);
	
	for (int i = 0; i < ComplexAttackRowNames.Num(); i++) {
		if (AvatarComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttackRowNames[i], ContextString)->Name == AttackName) {
			AttackData = *AvatarComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttackRowNames[i], ContextString);
			break;
		}
	}

	if (Attacker->CurrentSelectedAttack.AttackCategory == EBattle_AttackCategories::Offensive && Attacker->CurrentSelectedAttack.BasePower > 0) {
		// Calculate Damage
		CurrentDamage += ((Attacker->AvatarData.BattleStats.Attack * Attacker->CurrentSelectedAttack.BasePower) / Cast<ACharacter_Pathfinder>(Target)->AvatarData.BattleStats.Defence) / 2;

		if (CurrentDamage < 1.f) {
			CurrentDamage = 1.f;
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
}


void AStarmark_GameMode::EndOfBattle_Implementation()
{
	// Implemented in Blueprints
}
