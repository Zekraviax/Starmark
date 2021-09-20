#include "Starmark_GameMode.h"

#include "Actor_GridTile.h"
#include "AttackEffects_FunctionLibrary.h"
#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "PlayerPawn_Static.h"
#include "PlayerPawn_Flying.h"
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
		for (int j = 1; j <= 4; j++) {
			Server_SpawnAvatar(PlayerControllerReferences[i], j);
		}
	}

	// Only set the turn order once all Avatars are spawned
	for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
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
				AssembledTurnOrderText.Append(GameStateReference->AvatarTurnOrder[i]->AvatarData.AvatarName + " / ?\n");
		}

		GameStateReference->CurrentTurnOrderText = AssembledTurnOrderText;

		for (int i = 0; i < PlayerControllerReferences.Num(); i++) {
			PlayerControllerReferences[i]->SetBattleWidgetVariables();
			PlayerControllerReferences[i]->Client_GetTurnOrderText(GameStateReference->CurrentTurnOrderText);
		}

		Server_UpdateAllAvatarDecals();

		// Set first player to act
		GameStateReference->AvatarTurnOrder[0]->PlayerControllerReference->IsCurrentlyActingPlayer = true;
	}
}


void AStarmark_GameMode::Server_SpawnAvatar_Implementation(APlayerController_Base* PlayerController, int IndexInPlayerParty)
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

	AvatarRowNames = AvatarDataTable->GetRowNames();
	NewAvatarActor->AvatarData = *AvatarDataTable->FindRow<FAvatar_Struct>(AvatarRowNames[FMath::RandRange(0, AvatarRowNames.Num() - 1)], ContextString);

	// Avatar Stats
	NewAvatarActor->AvatarData.CurrentHealthPoints = NewAvatarActor->AvatarData.BaseStats.HealthPoints;
	NewAvatarActor->AvatarData.CurrentManaPoints = NewAvatarActor->AvatarData.BaseStats.ManaPoints;
	NewAvatarActor->AvatarData.CurrentTileMoves = NewAvatarActor->AvatarData.MaximumTileMoves;

	// MultiplayerUniqueID
	NewAvatarActor->PlayerControllerReference = PlayerController;
	NewAvatarActor->MultiplayerControllerUniqueID = PlayerController->MultiplayerUniqueID;

	//Add Simple Attacks First, then Complex Attacks
	if (NewAvatarActor->AvatarData.SimpleAttacks.Num() > 0) {
		for (int i = 0; i < NewAvatarActor->AvatarData.SimpleAttacks.Num(); i++) {
			NewAvatarActor->CurrentKnownAttacks.Add(*AvatarSimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(NewAvatarActor->AvatarData.SimpleAttacks[i].RowName, ContextString));
		}
	}

	if (NewAvatarActor->AvatarData.AttacksLearnedByBuyingWithEssence.Num() > 0) {
		for (int i = 0; i < NewAvatarActor->AvatarData.AttacksLearnedByBuyingWithEssence.Num(); i++) {
			if (NewAvatarActor->CurrentKnownAttacks.Num() < 4)
				NewAvatarActor->CurrentKnownAttacks.Add(*AvatarComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(NewAvatarActor->AvatarData.AttacksLearnedByBuyingWithEssence[i].RowName, ContextString));
		}
	}

	// Sent data to Clients
	NewAvatarActor->Client_GetAvatarData(NewAvatarActor->AvatarData);

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
		bool IsCurrentlyActing = false;

		if (CurrentlyActingAvatar == Avatars[i])
			IsCurrentlyActing = true;

		FoundActor->MultiplayerControllerUniqueID = FoundActor->PlayerControllerReference->MultiplayerUniqueID;

		for (int j = 0; j < PlayerControllerReferences.Num(); j++) {
			PlayerControllerReferences[j]->GetAvatarUpdateFromServer(FoundActor, FoundActor->MultiplayerControllerUniqueID, IsCurrentlyActing, true);
		}
	}
}


void AStarmark_GameMode::Server_LaunchAttack_Implementation(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Target)
{
	FAvatar_UltimateTypeChart* MoveTypeChartRow;
	FAvatar_UltimateTypeChart* TargetTypeChartRow;
	FString ContextString, MoveTypeAsString, TargetTypeAsString;
	int CurrentDamage = 1;

	MoveTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(Attacker->CurrentSelectedAttack.Type).ToString();
	TargetTypeAsString = UEnum::GetDisplayValueAsText<EAvatar_Types>(Target->AvatarData.PrimaryType).ToString();

	// Check for type advantage or disadvantage
	MoveTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*MoveTypeAsString), ContextString);
	TargetTypeChartRow = UltimateTypeChartDataTable->FindRow<FAvatar_UltimateTypeChart>(FName(*TargetTypeAsString), ContextString);

	// Attack Damage Formula
	CurrentDamage = FMath::CeilToInt(Attacker->AvatarData.BaseStats.Attack * Attacker->CurrentSelectedAttack.BasePower);
	CurrentDamage = FMath::CeilToInt(CurrentDamage / Target->AvatarData.BaseStats.Defence);
	CurrentDamage = FMath::CeilToInt((Attacker->AvatarData.BaseStats.Power / 2) * CurrentDamage);
	//CurrentDamage = FMath::CeilToInt(CurrentDamage / 8);

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

	// Subtract Health
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(Attacker->PlayerControllerReference->PlayerState);
	PlayerStateReference->Server_SubtractHealth_Implementation(Target, CurrentDamage);

	// Apply move effects after the damage has been dealt
	for (int i = 0; i < Attacker->CurrentSelectedAttack.AttackEffectsOnTarget.Num(); i++) {
		UAttackEffects_FunctionLibrary::SwitchOnAttackEffect(Attacker->CurrentSelectedAttack.AttackEffectsOnTarget[i], Attacker, Target);
	}

	// Tell the server to update everyone
	Server_UpdateAllAvatarDecals();
}


void AStarmark_GameMode::EndOfBattle_Implementation()
{
	// Implemented in Blueprints
}