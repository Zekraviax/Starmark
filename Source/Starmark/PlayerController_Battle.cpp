#include "PlayerController_Battle.h"


#include "Actor_AttackEffectsLibrary.h"
#include "Actor_GridTile.h"
#include "Actor_WorldGrid.h"
#include "AIController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character_HatTrick.h"
#include "Engine/World.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameMode.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "NavigationSystem.h"
#include "Widget_HUD_Battle.h"
#include "Widget_ServerHost.h"
#include "WidgetComponent_AvatarBattleData.h"


APlayerController_Battle::APlayerController_Battle()
{
	bShowMouseCursor = true;

	IsCurrentlyActingPlayer = false;
	PlayerClickMode = E_PlayerCharacter_ClickModes::E_Nothing;

	// Multiplayer
	bReplicates = true;
}


// To-Do: Figure out if any of these don't need to be replicated so they can be cleaned up
void APlayerController_Battle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 

	DOREPLIFETIME(APlayerController_Battle, CurrentSelectedAvatar);
	DOREPLIFETIME(APlayerController_Battle, BattleWidgetReference);
	DOREPLIFETIME(APlayerController_Battle, IsCurrentlyActingPlayer);
	DOREPLIFETIME(APlayerController_Battle, PlayerClickMode);
	DOREPLIFETIME(APlayerController_Battle, PlayerParty);
	DOREPLIFETIME(APlayerController_Battle, PlayerDataStruct);
	DOREPLIFETIME(APlayerController_Battle, IsReadyToStartMultiplayerBattle);
	DOREPLIFETIME(APlayerController_Battle, MultiplayerUniqueID);
	DOREPLIFETIME(APlayerController_Battle, PlayerName);
}


void APlayerController_Battle::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
}


void APlayerController_Battle::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// To-Do: Optimize this by taking it out of the Tick() function
	SetBattleWidgetVariables();
}


// ------------------------- Widgets
void APlayerController_Battle::Client_ClearLobbyFromScreen_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_ClearLobbyFromScreen / Clear lobby widgets on screen."));

	TArray<UUserWidget*> FoundServerHostWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundServerHostWidgets, UWidget_ServerHost::StaticClass(), true);

	for (int i = 0; i < FoundServerHostWidgets.Num(); i++) {
		FoundServerHostWidgets[i]->RemoveFromParent();
	}
}


void APlayerController_Battle::CreateBattleWidget_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / CreateBattleWidget / IsValid(BattleWidgetChildClass) returns: %s"), IsValid(BattleWidgetChildClass) ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / CreateBattleWidget / IsValid(BattleWidgetReference) returns: %s"), IsValid(BattleWidgetReference) ? TEXT("true") : TEXT("false"));

	if (BattleWidgetChildClass && !BattleWidgetReference) {
		BattleWidgetReference = CreateWidget<UWidget_HUD_Battle>(this, BattleWidgetChildClass);

		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / CreateBattleWidget / Created BattleWidgetReference. IsValid(BattleWidgetReference) returns: %s"), IsValid(BattleWidgetReference) ? TEXT("true") : TEXT("false"));

		if (IsValid(BattleWidgetReference)) {
			Local_BattleWidget_AddToScreen();
		}
	}
}


void APlayerController_Battle::SetPlayerClickMode(E_PlayerCharacter_ClickModes NewClickMode)
{
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(PlayerState);

	// First, set the new mode...
	PlayerClickMode = NewClickMode;

	// ...then handle secondary functionality.
	if (PlayerClickMode == E_PlayerCharacter_ClickModes::E_Nothing || PlayerClickMode == E_PlayerCharacter_ClickModes::E_MoveCharacter ||
		PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack || PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToControl) {
		
		// Make sure that the UI has the list of attacks.
		BattleWidgetReference->UpdateAvatarAttacksComponents(CurrentSelectedAvatar->CurrentKnownAttacks);
	} else if (PlayerClickMode == E_PlayerCharacter_ClickModes::SelectReserveAvatarToSummon) {
		// Replace the list of attacks with the list of reserve avatars.
		TArray<FAvatar_Struct> ReserveAvatars;
		// Don't need to pass-by-reference here since we're not changing anything.
		TArray<FAvatar_Struct> Team = PlayerStateReference->GetPlayerDataFromGameInstance().CurrentAvatarTeam;

		// Need to check if the avatar is in reserve *and* not already summoned to the field.
		TArray<FAvatar_Struct> TemporaryPlayerTeamCopy = PlayerDataStruct.CurrentAvatarTeam;
		TArray<AActor*> CharactersArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), CharactersArray);
		for (int j = 0; j < CharactersArray.Num(); j++) {
			TemporaryPlayerTeamCopy.Remove(Cast<ACharacter_Pathfinder>(CharactersArray[j])->AvatarData);
		}
		
		for (int i = 0; i < TemporaryPlayerTeamCopy.Num(); i++) {
			if (TemporaryPlayerTeamCopy[i].IndexInPlayerLibrary >= 4) {
				ReserveAvatars.Add(TemporaryPlayerTeamCopy[i]);
			}
		}
		
		BattleWidgetReference->SetCommandsToListOfReserveAvatars(ReserveAvatars);
	}
}


void APlayerController_Battle::Local_BattleWidget_AddToScreen() const
{
	BattleWidgetReference->AddToViewport();
	
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Local_BattleWidget_AddToScreen / Battle widget added to viewport."));
}


void APlayerController_Battle::SetBattleWidgetVariables()
{
	if (IsValid(BattleWidgetReference) && IsValid(CurrentSelectedAvatar)) {
		if (BattleWidgetReference->PlayerControllerReference != this) {
			BattleWidgetReference->PlayerControllerReference = this;
		}

		// To-Do: Fix these
		// Also To-Do: Figure out how and why these are broken before we fix them
		//BattleWidgetReference->AvatarBattleDataWidget->UpdateAvatarData(CurrentSelectedAvatar->AvatarData);
		BattleWidgetReference->AvatarBattleDataWidget->GetAvatarStatusEffects(CurrentSelectedAvatar->CurrentStatusEffectsArray);
	}
}


// ------------------------- Avatar
void APlayerController_Battle::OnRepNotify_CurrentSelectedAvatar_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / OnRepNotify_CurrentSelectedAvatar / Player %s has selected an avatar"), *PlayerDataStruct.PlayerName);
	
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(PlayerState);

	// (Default) Player party initialization
	if (PlayerStateReference) {
		// Avatar initialization
		if (CurrentSelectedAvatar) {
			UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / OnRepNotify_CurrentSelectedAvatar / Run the CurrentSelectedAvatar begin play function"));
			CurrentSelectedAvatar->BeginPlayWorkaroundFunction_Implementation(BattleWidgetReference);

			// Widget initialization
			if (!IsValid(BattleWidgetReference)) {
				CreateBattleWidget();
			}

			if (IsValid(BattleWidgetReference)) {
				SetBattleWidgetVariables();
			}

			UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / OnRepNotify_CurrentSelectedAvatar / Is %s ready to start the battle? %s"), *PlayerDataStruct.PlayerName, IsReadyToStartMultiplayerBattle ? TEXT("true") : TEXT("false"));
			Server_SetReadyToStartMultiplayerBattle();
		} else {
			UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / OnRepNotify_CurrentSelectedAvatar / CurrentSelectedAvatar not found, trying again..."));

			GetWorld()->GetTimerManager().SetTimer(PlayerStateTimerHandle, this, &APlayerController_Battle::OnRepNotify_CurrentSelectedAvatar, 0.5f, false);
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / OnRepNotify_CurrentSelectedAvatar / PlayerStateReference not found, trying again..."));

		GetWorld()->GetTimerManager().SetTimer(PlayerStateTimerHandle, this, &APlayerController_Battle::OnRepNotify_CurrentSelectedAvatar, 0.5f, false);
	}
}


void APlayerController_Battle::Server_SetReadyToStartMultiplayerBattle_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Server_SetReadyToStartMultiplayerBattle / Player %s is ready to start the multiplayer battle"), *PlayerDataStruct.PlayerName);
	
	IsReadyToStartMultiplayerBattle = true;
}


void APlayerController_Battle::ClientSendDataToServer_Implementation()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	PlayerDataStruct = GameInstanceReference->PlayerDataStruct;
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / ClientSendDataToServer / Sending player data with ProfileName: %s"), *PlayerDataStruct.ProfileName);

	ServerSendDataToServer(PlayerDataStruct);
}


void APlayerController_Battle::ServerSendDataToServer_Implementation(FPlayer_Data ReceivedPlayerDataStruct)
{
	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / ServerSendDataToServer / Received player data with ProfileName: %s"), *ReceivedPlayerDataStruct.ProfileName);

		Cast<AStarmark_PlayerState>(PlayerState)->PreBattleCheck = true;
		Cast<AStarmark_GameState>(GetWorld()->GetGameState())->PlayerDataStructsArray.Add(ReceivedPlayerDataStruct);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / ServerSendDataToServer / Error: This object does not have server authority."));
	}
}


// ------------------------- Battle
FPlayer_Data APlayerController_Battle::Server_GetDataFromProfile()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Server_GetDataFromProfile / Client is attempting to get the player's data from their profile"));
	
	// ReSharper disable once CppLocalVariableMayBeConst
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	PlayerDataStruct = GameInstanceReference->PlayerDataStruct;

	// Pass the data to the client's PlayerState
	Cast<AStarmark_PlayerState>(PlayerState)->PlayerData = PlayerDataStruct;

	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Server_GetDataFromProfile / PlayerName is: %s"), *PlayerDataStruct.PlayerName);
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Server_GetDataFromProfile / PlayerName has %d avatars"), PlayerDataStruct.CurrentAvatarTeam.Num());

	return PlayerDataStruct;
}


void APlayerController_Battle::OnMouseCursorBeginHover(ACharacter_Pathfinder* ActingAvatar, TArray<FVector> PathBetweenAvatars)
{
	/*
	TArray<AActor*> FoundAvatars, FoundGridTiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundAvatars);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTiles);

	for (int x = 0; x < FoundAvatars.Num(); x++) {
		Cast<ACharacter_Pathfinder>(FoundAvatars[x])->SetActorHighlightProperties(false, E_GridTile_ColourChangeContext::Normal);
	}

	for (int y = 0; y < FoundGridTiles.Num(); y++) {
		Cast<AActor_GridTile>(FoundGridTiles[y])->SetTileHighlightProperties(false, true, E_GridTile_ColourChangeContext::Normal);
	}

	for (auto Actor : ActingAvatar->ValidAttackTargetsArray) {
		if (Cast<ACharacter_Pathfinder>(Actor)) {
			Cast<ACharacter_Pathfinder>(Actor)->SetActorHighlightProperties(true, E_GridTile_ColourChangeContext::WithinAttackRange);
		} else if (Cast<AActor_GridTile>(Actor)) {
			Cast<AActor_GridTile>(Actor)->SetTileHighlightProperties(true, false, E_GridTile_ColourChangeContext::WithinAttackRange);
		}
	}
	*/

	TArray<AActor*> WorldGridArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_WorldGrid::StaticClass(), WorldGridArray);
	AActor_WorldGrid* WorldGridRef = Cast<AActor_WorldGrid>(WorldGridArray[0]);
	WorldGridRef->CustomAStarPathfindingAlgorithm(ActingAvatar->GetActorLocation(), PathBetweenAvatars.Last());
}


void APlayerController_Battle::OnPrimaryClick(AActor* ClickedActor, TArray<AActor*> ValidTargetsArray)
{
	TArray<AActor*> AttackEffectsLibraries;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_AttackEffectsLibrary::StaticClass(), AttackEffectsLibraries);

	CurrentSelectedAvatar->ValidAttackTargetsArray = ValidTargetsArray;

	if (ClickedActor->IsValidLowLevel()) {
		// The Hat Trick functions should take priority over other moves
		if (ClickedActor->GetClass()->GetFullName().Contains("HatTrick")) {
			// Hide avatar
			Cast<ACharacter_HatTrick>(ClickedActor)->IsOwnerHiddenInThisHat = true;
			CurrentSelectedAvatar->SetActorLocation(ClickedActor->GetActorLocation());
			CurrentSelectedAvatar->SetActorHiddenInGame(true);

			// End avatar turn
			Cast<AStarmark_GameState>(GetWorld()->GetGameState())->AvatarEndTurn();
		} else if (CurrentSelectedAvatar->CurrentSelectedAttack.AttackEffectsOnTarget.Contains(EBattle_AttackEffects::SpawnHats)) {
			if (AttackEffectsLibraries.Num() <= 0) {
				AttackEffectsLibrary_Reference = GetWorld()->SpawnActor<AActor_AttackEffectsLibrary>(AttackEffectsLibrary_Class);
				UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / Spawn AttackEffectsLibrary_Reference"));
			}

			if (AttackEffectsLibrary_Reference->HatTilesArray.Num() < 3) {
				if (Cast<AActor_GridTile>(ClickedActor)) {
					AttackEffectsLibrary_Reference->HatTilesArray.Add(Cast<AActor_GridTile>(ClickedActor));
					UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / Add selected tile to HatTilesArray"));
				}
			}
		}
		// All other attacks
		else if (CurrentSelectedAvatar->CurrentSelectedAttack.Name != "Default" && CurrentSelectedAvatar->CurrentSelectedAttack.Name != "None" && CurrentSelectedAvatar->CurrentSelectedAttack.Name != "---" && ValidTargetsArray.Num() > 0) {
			// Subtract attack's MP cost
			CurrentSelectedAvatar->AvatarData.CurrentManaPoints -= CurrentSelectedAvatar->CurrentSelectedAttack.ManaCost;

			if (ClickedActor && CurrentSelectedAvatar->CurrentSelectedAttack.AttackTargetsInRange == EBattle_AttackTargetsInRange::AttackClickedAvatar) {
				// Find an entity amongst the valid targets
				for (AActor* Actor : ValidTargetsArray) {
					if (Cast<ACharacter_Pathfinder>(Actor)) {
						ClickedActor = Cast<ACharacter_Pathfinder>(Actor);
						break;
					}
				}

				// If we're attacking, and we clicked on a valid target in-range, launch an attack
				UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / Launch attack against ClickedActor"));
				UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / ClickedActor is: %s"), *ClickedActor->GetFullName());
				
				if (Cast<ACharacter_Pathfinder>(ClickedActor)) {
					if (CurrentSelectedAvatar->ValidAttackTargetsArray.Contains(ClickedActor)) {
						CurrentSelectedAvatar->LaunchAttack_Implementation(Cast<ACharacter_Pathfinder>(ClickedActor));
						// Don't automatically end the turn just because someone attacked.
						// End the turn automatically if no avatars died.
						//Client_SendEndOfTurnCommandToServer();
					}
				}
			} else if (CurrentSelectedAvatar->CurrentSelectedAttack.AttackTargetsInRange == EBattle_AttackTargetsInRange::AttackAllTargets) {
				UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / Launch attack against all valid targets in range: %d"), CurrentSelectedAvatar->ValidAttackTargetsArray.Num());

				for (int i = 0; i < CurrentSelectedAvatar->ValidAttackTargetsArray.Num(); i++) {
					if (Cast<ACharacter_Pathfinder>(CurrentSelectedAvatar->ValidAttackTargetsArray[i])) {
						UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / AttackTarget is: %s"), *CurrentSelectedAvatar->ValidAttackTargetsArray[i]->GetFullName());
						CurrentSelectedAvatar->LaunchAttack_Implementation(Cast<ACharacter_Pathfinder>(CurrentSelectedAvatar->ValidAttackTargetsArray[i]));
					}
				}

				//Client_SendEndOfTurnCommandToServer();
			}
		}
	}
}


void APlayerController_Battle::HighlightSpecificAvatarsAndTiles(const TArray<ACharacter_Pathfinder*>& Avatars, const TArray<AActor_GridTile*>& Tiles) const
{
	TArray<AActor*> FoundAvatars, FoundGridTiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundAvatars);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), FoundGridTiles);

	for (int x = 0; x < FoundAvatars.Num(); x++) {
		if (Avatars.Contains(FoundAvatars[x])) {
			Cast<ACharacter_Pathfinder>(FoundAvatars[x])->SetActorHighlightProperties(true, E_GridTile_ColourChangeContext::WithinAttackRange);
		} else {
			Cast<ACharacter_Pathfinder>(FoundAvatars[x])->SetActorHighlightProperties(false, E_GridTile_ColourChangeContext::Normal);
		}
	}

	for (int y = 0; y < FoundGridTiles.Num(); y++) {
		if (Tiles.Contains(FoundGridTiles[y])) {
			Cast<AActor_GridTile>(FoundGridTiles[y])->SetTileHighlightProperties(true, false, E_GridTile_ColourChangeContext::WithinAttackRange);
		} else {
			Cast<AActor_GridTile>(FoundGridTiles[y])->SetTileHighlightProperties(false, true, E_GridTile_ColourChangeContext::Normal);
		}
	}
}


void APlayerController_Battle::BeginSelectingTileForReserveAvatar(int SelectedBattleUniqueID)
{
	// These are the arrays of entities and grid tiles we're going to highlight at the end of the function.
	TArray<ACharacter_Pathfinder*> AvatarsArray;
	TArray<AActor_GridTile*> GridTilesArray;
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(PlayerState);

	TArray<AActor*> WorldGridArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_WorldGrid::StaticClass(), WorldGridArray);
	AActor_WorldGrid* WorldGridRef = Cast<AActor_WorldGrid>(WorldGridArray[0]);

	// Highlight the currently acting avatar if it isn't the end of the turn and an avatar was defeated.
	// Otherwise, highlight each valid tile that the player can summon avatars to.
	if (PlayerStateReference->NumberOfAvatarsDiedThisTurn > 0) {
		TArray<AActor*> GridTileActorsArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTileActorsArray);

		for (AActor* TileAsActor : GridTileActorsArray) {
			AActor_GridTile* Tile = Cast<AActor_GridTile>(TileAsActor);

			if (Tile) {
				if (!Tile->Properties.Contains(E_GridTile_Properties::E_Occupied)) {
					if (Tile->AssignedMultiplayerUniqueID == MultiplayerUniqueID) {
						GridTilesArray.AddUnique(Tile);
					} else if (WorldGridRef->GetTotalDistanceBetweenTwoPositions(Tile->GetActorLocation(), CurrentSelectedAvatar->GetActorLocation()) == 1) {
						GridTilesArray.AddUnique(Tile);
					} else if (Tile->AssignedMultiplayerUniqueID == MultiplayerUniqueID) {
						GridTilesArray.AddUnique(Tile);
					}
				}
			}
		}
	} else {
		AvatarsArray.Add(CurrentSelectedAvatar);
		GridTilesArray.Add(Cast<AActor_GridTile>(WorldGridRef->GetAndReturnGridTileAtLocation(WorldGridRef->ConvertGridTileLocationToCoordinates(CurrentSelectedAvatar->GetActorLocation()))));
	}

	// Remember which avatar was chosen.
	SelectedReserveAvatarBattleUniqueID = SelectedBattleUniqueID;

	// Set the players' mouse mode to select a tile and finalize the summoning.
	PlayerClickMode = E_PlayerCharacter_ClickModes::SummonAvatar;
	HighlightSpecificAvatarsAndTiles(AvatarsArray, GridTilesArray);
}


void APlayerController_Battle::SummonReserveAvatarAtSelectedTile_Implementation(AActor_GridTile* SelectedTile, ACharacter_Pathfinder* SelectedAvatar)
{
	int ReserveAvatarDataIndex = -1;

	// If a tile is selected, that means the game mode needs to spawn in a new avatar.
	// Otherwise, we can just override the existing data on an avatar.
	if (SelectedTile) {
		for (int i = 0; i < PlayerDataStruct.CurrentAvatarTeam.Num(); i++) {
			if (PlayerDataStruct.CurrentAvatarTeam[i].IndexInPlayerLibrary == SelectedReserveAvatarBattleUniqueID) {
				UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / SummonReserveAvatarAtSelectedTile / Summoning avatar %s within Unique ID %d."), *PlayerDataStruct.CurrentAvatarTeam[i].Nickname, SelectedReserveAvatarBattleUniqueID);
				
				Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Server_SpawnAvatar(this, PlayerDataStruct, SelectedReserveAvatarBattleUniqueID, PlayerDataStruct.CurrentAvatarTeam[i], SelectedTile->GetActorLocation());
				break;
			}
		}
		
		// Once an avatar has been spawned, we can set it to the FoundAvatar local variable?
	} else {
		FAvatar_Struct ReserveAvatarData;
		ACharacter_Pathfinder* FoundAvatar = SelectedAvatar;

		// Swap indices.
		// We need to retrieve the index of the chosen reserve avatar, then find that avatars' data here.
		for (int i = 0; i < PlayerDataStruct.CurrentAvatarTeam.Num(); i++) {
			if (PlayerDataStruct.CurrentAvatarTeam[i].IndexInPlayerLibrary == SelectedReserveAvatarBattleUniqueID) {
				for (int j = 0; j < PlayerDataStruct.CurrentAvatarTeam.Num(); j++) {
					if (PlayerDataStruct.CurrentAvatarTeam[j].IndexInPlayerLibrary == FoundAvatar->AvatarData.IndexInPlayerLibrary) {
						// Step one: Get the reserve Avatars' data and set it aside.
						ReserveAvatarDataIndex = PlayerDataStruct.CurrentAvatarTeam[i].IndexInPlayerLibrary;
						ReserveAvatarData = PlayerDataStruct.CurrentAvatarTeam[i];

						// Step two: Override the reserve Avatars' data with the current avatars' data.
						PlayerDataStruct.CurrentAvatarTeam[i].IndexInPlayerLibrary = PlayerDataStruct.CurrentAvatarTeam[j].IndexInPlayerLibrary;
						PlayerDataStruct.CurrentAvatarTeam[j].IndexInPlayerLibrary = ReserveAvatarDataIndex;

						// Step three: Override the physical Avatar actors' data with the reserve avatars' data.
						FoundAvatar->AvatarData = ReserveAvatarData;
					}
				}
			
				PlayerDataStruct.CurrentAvatarTeam[i] = FoundAvatar->AvatarData;
				ReserveAvatarData = PlayerDataStruct.CurrentAvatarTeam[i];

				FoundAvatar->AvatarData = ReserveAvatarData;
				FoundAvatar->AvatarData.IndexInPlayerLibrary = ReserveAvatarDataIndex;
				break;
			}
		}

		// New avatar's attacks
		FoundAvatar->CurrentKnownAttacks.Empty();
		UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(GetGameInstance());
		for (int i = 0; i < FoundAvatar->AvatarData.CurrentEquippedAttackNames.Num(); i++) {
			FoundAvatar->CurrentKnownAttacks.Add(*GameInstanceReference->ReturnAttackFromDataTable(FoundAvatar->AvatarData.CurrentEquippedAttackNames[i]));
		}

		// Set health and mana?
	}
	
	// The GameMode will handle checking whether or not all players have finished summoning reserve avatars.
	Cast<AStarmark_PlayerState>(PlayerState)->NumberOfAvatarsDiedThisTurn--;
	Client_SendEndOfTurnCommandToServer();
}


void APlayerController_Battle::DelayedEndTurn()
{
	GetWorld()->GetTimerManager().SetTimer(PlayerStateTimerHandle, this, &APlayerController_Battle::OnRepNotify_CurrentSelectedAvatar, 1.f, false);
}


void APlayerController_Battle::Client_SendEndOfTurnCommandToServer_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_SendEndOfTurnCommandToServer / Call SendEndOfTurnCommandToServer()"));
	
	CurrentSelectedAvatar->CurrentSelectedAttack.Name = "Default";
	CurrentSelectedAvatar->CurrentSelectedAttack.AttackPattern = EBattle_AttackPatterns::Circle;

	SendEndOfTurnCommandToServer();
}


void APlayerController_Battle::SendEndOfTurnCommandToServer_Implementation()
{
	// Reset the players' HUD here.
	SetPlayerClickMode(E_PlayerCharacter_ClickModes::E_MoveCharacter);
	
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / SendEndOfTurnCommandToServer / Call AvatarEndTurn_Implementation()"));
	Cast<AStarmark_GameState>(GetWorld()->GetGameState())->AvatarEndTurn_Implementation();
}


void APlayerController_Battle::Player_OnAvatarTurnChanged_Implementation()
{
	TArray<AActor*> GridTilesArray;
	SetBattleWidgetVariables();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	for (int j = 0; j < GridTilesArray.Num(); j++) {
		Cast<AActor_GridTile>(GridTilesArray[j])->SetTileHighlightProperties(false, true, E_GridTile_ColourChangeContext::Normal);
	}
}


void APlayerController_Battle::Client_UpdateAttacksInHud_Implementation(const ACharacter_Pathfinder* ActingAvatar)
{
	if (CurrentSelectedAvatar && ActingAvatar) {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateAttacksInHud_Implementation / Passed avatar: %s"), *ActingAvatar->AvatarData.Nickname);

		if (BattleWidgetReference) {
			UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateAttacksInHud_Implementation / Initializing HUD"));

			if (ActingAvatar->CurrentKnownAttacks.Num() > 0) {
				BattleWidgetReference->UpdateAvatarAttacksComponents(ActingAvatar->CurrentKnownAttacks);
			}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateAttacksInHud_Implementation / Error: HUD reference is not valid"));
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateAttacksInHud_Implementation / CurrentSelectedAvatar or ActingAvatar is not valid"));
	}
}


void APlayerController_Battle::Client_ShowHideHud_Implementation(bool ShowHud)
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_ShowHideHud / Show HUD? %s"), ShowHud ? TEXT("true") : TEXT("false"));

	BattleWidgetReference->ShowHideActingPlayerHudElements(ShowHud);
}


// ------------------------- Multiplayer Battle
void APlayerController_Battle::GetAvatarUpdateFromServer_Implementation(ACharacter_Pathfinder* AvatarReference, int AvatarUniqueID, bool IsCurrentlyActing, bool IsCurrentlSelectedAvatar)
{
	if (IsValid(AvatarReference)) {
		LocalAvatarUpdate(AvatarReference, AvatarUniqueID, IsCurrentlyActing, IsCurrentlSelectedAvatar);
	}
}


void APlayerController_Battle::Client_UpdateCurrentAvatarInHud_Implementation(ACharacter_Pathfinder* ActingAvatar)
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateCurrentAvatarInHud / Received current acting avatar: %s"), *ActingAvatar->AvatarData.Nickname);

	BattleWidgetReference->SetCurrentActingEntityInfo(ActingAvatar);
	BattleWidgetReference->AvatarBattleDataWidget->UpdateAvatarData(CurrentSelectedAvatar->AvatarData);
}


void APlayerController_Battle::Client_UpdateCurrentTurnOrderInHud_Implementation(const TArray<UTexture2D*>& InDynamicAvatarTurnOrderImages)
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateCurrentTurnOrderInHud / Received %d turn order images."), InDynamicAvatarTurnOrderImages.Num());
	
	BattleWidgetReference->SetUiIconsInTurnOrder(InDynamicAvatarTurnOrderImages);
}

void APlayerController_Battle::LocalAvatarUpdate(ACharacter_Pathfinder* AvatarReference, int AvatarUniqueID, bool IsCurrentlyActing, bool IsCurrentlSelectedAvatar)
{
	ACharacter_Pathfinder* FoundActor = AvatarReference;

	FoundActor->ActorSelectedPlane->SetHiddenInGame(!IsCurrentlyActing);
	FoundActor->ActorSelectedDynamicMaterialColourUpdate();

	SetBattleWidgetVariables();
}


void APlayerController_Battle::Client_SendLaunchAttackToServer_Implementation(ACharacter_Pathfinder* Attacker, AActor* Target, const FString& AttackName)
{
	UE_LOG(LogTemp, Warning, TEXT("Client_SendLaunchAttackToServer / Attack chosen: %s"), *AttackName);
	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Server_LaunchAttack(Attacker, Target, AttackName);
}
