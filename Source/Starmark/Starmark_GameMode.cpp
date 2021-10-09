#include "Starmark_GameMode.h"

#include "Actor_AttackEffectsLibrary.h"
#include "Actor_GridTile.h"
#include "AttackEffects_FunctionLibrary.h"
#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "PlayerPawn_Static.h"
#include "PlayerPawn_Flying.h"
#include "Player_SaveData.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Starmark_Variables.h"
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

	NewPlayerController->Possess(GetWorld()->SpawnActor<APlayerPawn_Flying>(PlayerPawnBlueprintClass, Location, Rotation, SpawnInfo));

	MultiplayerUniqueIDCounter++;
	NewPlayerController->MultiplayerUniqueID = MultiplayerUniqueIDCounter;
	UE_LOG(LogTemp, Warning, TEXT("OnPlayerPostLogin / MultiplayerUniqueIDCounter is: %d"), MultiplayerUniqueIDCounter);

	// Load player data
	//Cast<UStarmark_GameInstance>(NewPlayerController->GetGameInstance())->LoadProfile(Cast<UStarmark_GameInstance>(NewPlayerController->GetGameInstance())->CurrentProfileName);
	Cast<AStarmark_PlayerState>(NewPlayerController->PlayerState)->Server_UpdatePlayerData();
	Cast<AStarmark_PlayerState>(NewPlayerController->PlayerState)->Client_UpdateReplicatedPlayerName();

	PlayerControllerReferences.Add(NewPlayerController);
	UE_LOG(LogTemp, Warning, TEXT("OnPlayerPostLogin / PlayerControllerReferences found: %d"), PlayerControllerReferences.Num());

	// When all players have joined, begin running the functions needed to start the battle
	if (ExpectedPlayers >= 2 && PlayerControllerReferences.Num() >= ExpectedPlayers) {
		UE_LOG(LogTemp, Warning, TEXT("OnPlayerPostLogin / Call Server_BeginMultiplayerBattle()"));
		Server_BeginMultiplayerBattle();
	} else if (ExpectedPlayers == 1) {
		UE_LOG(LogTemp, Warning, TEXT("OnPlayerPostLogin / Call Server_SinglePlayerBeginMultiplayerBattle()"));
		Server_SinglePlayerBeginMultiplayerBattle(NewPlayerController);
	}
}


void AStarmark_GameMode::Server_BeginMultiplayerBattle_Implementation()
{
	AStarmark_GameState* GameStateReference = nullptr;
	GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
		PlayerControllerReferences[i]->Server_GetDataFromProfile();
		TArray<FAvatar_Struct> CurrentPlayerTeam = PlayerControllerReferences[i]->PlayerParty;

		int SpawnedAvatarCount = 0;

		for (int j = CurrentPlayerTeam.Num() - 1; j >= 0; j--) {
			if (CurrentPlayerTeam[j].AvatarName != "Default") {
				if (SpawnedAvatarCount < 4) {
					Server_SpawnAvatar(PlayerControllerReferences[i], j + 1, CurrentPlayerTeam[j]);
					UE_LOG(LogTemp, Warning, TEXT("OnPlayerPostLogin / Spawn Avatar %s for Player %s"), *CurrentPlayerTeam[j].AvatarName, *PlayerControllerReferences[i]->PlayerName);
					SpawnedAvatarCount++;
				}
			} else {
				Cast<UStarmark_GameInstance>(PlayerControllerReferences[i]->GetGameInstance())->CurrentProfileReference->CurrentAvatarTeam.RemoveAt(j);
				UE_LOG(LogTemp, Warning, TEXT("Server_BeginMultiplayerBattle / Remove invalid member %d from PlayerState_PlayerParty"), j);
			}
		}
	}

	Server_MultiplayerBattleCheckAllPlayersReady();
}


void AStarmark_GameMode::Server_SinglePlayerBeginMultiplayerBattle_Implementation(APlayerController_Base* PlayerControllerReference)
{
	AStarmark_GameState* GameStateReference = nullptr;
	GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	TArray<FAvatar_Struct> CurrentPlayerTeam = Cast<UStarmark_GameInstance>(PlayerControllerReferences[0]->GetGameInstance())->CurrentProfileReference->CurrentAvatarTeam;

	int SpawnedAvatarCount = 0;

	for (int j = CurrentPlayerTeam.Num() - 1; j >= 0; j--) {
		if (CurrentPlayerTeam[j].AvatarName != "Default") {
			if (SpawnedAvatarCount < 4) {
				Server_SpawnAvatar(PlayerControllerReferences[0], j + 1, Cast<UStarmark_GameInstance>(PlayerControllerReferences[0]->GetGameInstance())->CurrentProfileReference->CurrentAvatarTeam[j]);
				SpawnedAvatarCount++;
			}
		} else {
			Cast<UStarmark_GameInstance>(PlayerControllerReferences[0]->GetGameInstance())->CurrentProfileReference->CurrentAvatarTeam.RemoveAt(j);
			UE_LOG(LogTemp, Warning, TEXT("Server_SinglePlayerBeginMultiplayerBattle / Remove invalid member %d from PlayerState_PlayerParty"), j);
		}
	}

	Server_MultiplayerBattleCheckAllPlayersReady();
}


void AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady_Implementation()
{
	TArray<bool> ReadyStatuses;
	TArray<AActor*> GridTilesArray;
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
		ReadyStatuses.Add(PlayerControllerReferences[i]->IsReadyToStartMultiplayerBattle);
	}

	// Assemble turn order text
	if (ReadyStatuses.Contains(false) || ReadyStatuses.Num() < ExpectedPlayers) {
		GetWorld()->GetTimerManager().SetTimer(PlayerReadyCheckTimerHandle, this, &AStarmark_GameMode::Server_MultiplayerBattleCheckAllPlayersReady, 0.5f, false);
	} else {
		// Only set the turn order once all Avatars are spawned
		for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
			GameStateReference->SetTurnOrder(PlayerControllerReferences);
		}

		Server_AssembleTurnOrderText();

		for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
			for (int j = 0; j < GameStateReference->AvatarTurnOrder.Num(); j++) {
				if (PlayerControllerReferences[i]->MultiplayerUniqueID == GameStateReference->AvatarTurnOrder[j]->MultiplayerControllerUniqueID) {
					PlayerControllerReferences[i]->CurrentSelectedAvatar = GameStateReference->AvatarTurnOrder[j];
					break;
				}
			}

			PlayerControllerReferences[i]->SetBattleWidgetVariables();
		}

		Server_UpdateAllAvatarDecals();

		// Set all tiles to be occupied
		//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
		//for (int j = 0; j < GridTilesArray.Num(); j++) {
		//	Cast<AActor_GridTile>(GridTilesArray[j])->UpdateGridTileState();
		//}

		// Set first player to act
		GameStateReference->AvatarTurnOrder[0]->PlayerControllerReference->IsCurrentlyActingPlayer = true;
	}
}


void AStarmark_GameMode::Server_AssembleTurnOrderText_Implementation()
{
	FString NewTurnOrderText;
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	for (int i = 0; i < GameStateReference->AvatarTurnOrder.Num(); i++) {
		AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GameStateReference->AvatarTurnOrder[i]->PlayerControllerReference->PlayerState);

		if (PlayerStateReference) {
			if (PlayerStateReference->ReplicatedPlayerName == "" || PlayerStateReference->ReplicatedPlayerName.Len() <= 0)
				PlayerStateReference->ReplicatedPlayerName = ("Player " + FString::FromInt(GameStateReference->AvatarTurnOrder[i]->PlayerControllerReference->MultiplayerUniqueID));

			NewTurnOrderText.Append(GameStateReference->AvatarTurnOrder[i]->AvatarData.AvatarName + " / " + PlayerStateReference->ReplicatedPlayerName + "\n");
		}
		else
			NewTurnOrderText.Append(GameStateReference->AvatarTurnOrder[i]->AvatarData.AvatarName + " / ?\n");
	}

	GameStateReference->CurrentTurnOrderText = NewTurnOrderText;

	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
		PlayerControllerReferences[i]->Client_GetTurnOrderText(GameStateReference->CurrentTurnOrderText);
	}
}


void AStarmark_GameMode::Server_SpawnAvatar_Implementation(APlayerController_Base* PlayerController, int IndexInPlayerParty, FAvatar_Struct AvatarData)
{
	FString ContextString;
	FActorSpawnParameters SpawnInfo;
	TArray<AActor*> FoundGridTileActors, ValidMultiplayerSpawnTiles;
	TArray<FName> AvatarRowNames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTileActors);


	// Find all tiles that can spawn avatars in multiplayer battles
	for (int i = 0; i < FoundGridTileActors.Num(); i++) {
		AActor_GridTile* GridTileReference = Cast<AActor_GridTile>(FoundGridTileActors[i]);

		if (GridTileReference->Properties.Contains(E_GridTile_Properties::E_PlayerAvatarSpawn) &&
			GridTileReference->AssignedMultiplayerUniqueID == PlayerController->MultiplayerUniqueID &&
			GridTileReference->AvatarSlotNumber == IndexInPlayerParty) {
			ValidMultiplayerSpawnTiles.Add(FoundGridTileActors[i]);
		}
	}

	FVector Location = ValidMultiplayerSpawnTiles[0]->GetActorLocation();
	Location.Z = 95;

	ACharacter_Pathfinder* NewAvatarActor = GetWorld()->SpawnActor<ACharacter_Pathfinder>(AvatarBlueprintClass, Location, FRotator::ZeroRotator, SpawnInfo);
	NewAvatarActor->AvatarData = AvatarData;

	// Random Avatars
	//AvatarRowNames = AvatarDataTable->GetRowNames();
	//NewAvatarActor->AvatarData = *AvatarDataTable->FindRow<FAvatar_Struct>(AvatarRowNames[FMath::RandRange(0, AvatarRowNames.Num() - 1)], ContextString);

	// Avatar Stats
	NewAvatarActor->AvatarData.CurrentHealthPoints = NewAvatarActor->AvatarData.BaseStats.HealthPoints;
	NewAvatarActor->AvatarData.CurrentManaPoints = NewAvatarActor->AvatarData.BaseStats.ManaPoints;
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

	NewAvatarActor->SetTilesOccupiedBySize();
}


void AStarmark_GameMode::Server_UpdateAllAvatarDecals_Implementation()
{
	TArray<AActor*> Avatars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), Avatars);
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());
	ACharacter_Pathfinder* CurrentlyActingAvatar;

	if (GameStateReference->AvatarTurnOrder.IsValidIndex(GameStateReference->CurrentAvatarTurnIndex))
		CurrentlyActingAvatar = GameStateReference->AvatarTurnOrder[GameStateReference->CurrentAvatarTurnIndex];
	else 
		CurrentlyActingAvatar = GameStateReference->AvatarTurnOrder[0];

	for (int i = 0; i < Avatars.Num(); i++) {
		ACharacter_Pathfinder* FoundActor = Cast<ACharacter_Pathfinder>(Avatars[i]);
		bool IsCurrentlyActing = false;

		if (CurrentlyActingAvatar == Avatars[i])
			IsCurrentlyActing = true;

		FoundActor->MultiplayerControllerUniqueID = FoundActor->PlayerControllerReference->MultiplayerUniqueID;

		for (int j = 0; j < PlayerControllerReferences.Num(); j++) {
			PlayerControllerReferences[j]->GetAvatarUpdateFromServer(FoundActor, FoundActor->MultiplayerControllerUniqueID, IsCurrentlyActing, true);
		}
	}
}


void AStarmark_GameMode::Server_LaunchAttack_Implementation(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Target, const FString& AttackName)
{
	FAvatar_UltimateTypeChart* MoveTypeChartRow;
	FAvatar_UltimateTypeChart* TargetTypeChartRow;
	FAvatar_AttackStruct AttackData;
	FActorSpawnParameters SpawnInfo;
	TArray<FName> ComplexAttackRowNames = AvatarComplexAttacksDataTable->GetRowNames();
	FString ContextString, MoveTypeAsString, TargetTypeAsString;
	int CurrentDamage = 1;

	UE_LOG(LogTemp, Warning, TEXT("Server_LaunchAttack / Attack chosen: %s"), *AttackName);
	for (int i = 0; i < ComplexAttackRowNames.Num(); i++) {
		if (AvatarComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttackRowNames[i], ContextString)->Name == AttackName) {
			AttackData = *AvatarComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttackRowNames[i], ContextString);
			break;
		}
	}

	MoveTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(AttackData.Type).ToString();
	TargetTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(Target->AvatarData.PrimaryType).ToString();

	// Check for type advantage or disadvantage
	MoveTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*MoveTypeAsString), ContextString);
	TargetTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*TargetTypeAsString), ContextString);

	// Attack Damage Formula
	CurrentDamage = FMath::CeilToInt(Attacker->AvatarData.BaseStats.Attack * AttackData.BasePower);
	UE_LOG(LogTemp, Warning, TEXT("Server_LaunchAttack / Attacker's Attack * Attack's Base Power is: %d"), CurrentDamage);
	CurrentDamage = FMath::CeilToInt(CurrentDamage / Target->AvatarData.BaseStats.Defence);
	UE_LOG(LogTemp, Warning, TEXT("Server_LaunchAttack / Current Damage / Defender's Defence is: %d"), CurrentDamage);
	//CurrentDamage = FMath::CeilToInt((Attacker->AvatarData.BaseStats.Power) * CurrentDamage);
	CurrentDamage = FMath::CeilToInt(CurrentDamage / 6);
	UE_LOG(LogTemp, Warning, TEXT("Server_LaunchAttack / Current Damage / 6 is: %d"), CurrentDamage);

	// Compare each Move type against the Target type
	for (int j = 0; j < TargetTypeChartRow->CombinationTypes.Num(); j++) {
		// 2x Damage
		if (MoveTypeChartRow->DoesMoreDamageToTypes.Contains(TargetTypeChartRow->CombinationTypes[j]))
			CurrentDamage = CurrentDamage * 2;

		// 0.5x Damage
		else if (MoveTypeChartRow->DoesLessDamageToTypes.Contains(TargetTypeChartRow->CombinationTypes[j]))
			CurrentDamage = CurrentDamage / 2;
	}

	if (CurrentDamage < 1)
		CurrentDamage = 1;

	UE_LOG(LogTemp, Warning, TEXT("Server_LaunchAttack / Calculated damage is: %d"), CurrentDamage);

	// Subtract Health
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(Attacker->PlayerControllerReference->PlayerState);
	PlayerStateReference->Server_SubtractHealth_Implementation(Target, CurrentDamage);

	// Apply move effects after the damage has been dealt
	for (int i = 0; i < AttackData.AttackEffectsOnTarget.Num(); i++) {
		//UAttackEffects_FunctionLibrary::SwitchOnAttackEffect(AttackData.AttackEffectsOnTarget[i], Attacker, Target);
		if (!AttackEffectsLibrary_Reference && AttackEffectsLibrary_Class)
			AttackEffectsLibrary_Reference = GetWorld()->SpawnActor<AActor_AttackEffectsLibrary>(AttackEffectsLibrary_Class, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);

		AttackEffectsLibrary_Reference->SwitchOnAttackEffect(AttackData.AttackEffectsOnTarget[i], Attacker, Target);
	}

	// Tell the server to update everyone
	Server_UpdateAllAvatarDecals();

	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
		PlayerControllerReferences[i]->Player_OnAvatarTurnChanged();
	}
}


void AStarmark_GameMode::EndOfBattle_Implementation()
{
	// Implemented in Blueprints
}