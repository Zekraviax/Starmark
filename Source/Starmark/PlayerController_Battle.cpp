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
#include "Player_SaveData.h"
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


// To-Do: Figure out if any of these can be cleaned up
// (not replicated)
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
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_ClearLobbyFromScreen / Clear lobby widgets on screen"));

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
	// First, set the new mode
	PlayerClickMode = NewClickMode;

	// Then handle secondary functionality
	if (PlayerClickMode == E_PlayerCharacter_ClickModes::E_Nothing || PlayerClickMode == E_PlayerCharacter_ClickModes::E_MoveCharacter ||
		PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack || PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToControl) {
		// Make sure that the UI has the list of attack
		BattleWidgetReference->UpdateAvatarAttacksComponents(CurrentSelectedAvatar->CurrentKnownAttacks);
	} else if (PlayerClickMode == E_PlayerCharacter_ClickModes::SelectReserveAvatarToSummon) {
		// Replace the list of attacks with the list of reserve avatars
	}
}


void APlayerController_Battle::Local_BattleWidget_AddToScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Local_BattleWidget_AddToScreen / Battle widget added to viewport"));
	
	BattleWidgetReference->AddToViewport();
}


void APlayerController_Battle::SetBattleWidgetVariables()
{
	if (IsValid(BattleWidgetReference) && IsValid(CurrentSelectedAvatar)) {
		if (BattleWidgetReference->PlayerControllerReference != this) {
			BattleWidgetReference->PlayerControllerReference = this;
		}

		//UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / SetBattleWidgetVariables / Set variables in the battle widget"));

		// To-Do: Fix these
		// Also To-Do: Figure out how and why these are broken before we fix them
		BattleWidgetReference->AvatarBattleDataWidget->UpdateAvatarData(CurrentSelectedAvatar->AvatarData);
		BattleWidgetReference->AvatarBattleDataWidget->GetAvatarStatusEffects(CurrentSelectedAvatar->CurrentStatusEffectsArray);
	}
}


void APlayerController_Battle::Local_GetTurnOrderText(const FString& NewTurnOrderText) const
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Local_GetTurnOrderText / Update the turn order text in the battle widget"));
	
	BattleWidgetReference->UpdateTurnOrderText(NewTurnOrderText);
}


void APlayerController_Battle::Client_GetAvatarImagesInDynamicTurnOrder_Implementation()
{
	TArray<UTexture2D*> InDynamicAvatarTurnOrderImages = Cast<AStarmark_GameState>(GetWorld()->GetGameState())->DynamicAvatarTurnOrderImages;
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_GetAvatarImagesInDynamicTurnOrder / InDynamicAvatarTurnOrderImages child count: %d"), InDynamicAvatarTurnOrderImages.Num());

	BattleWidgetReference->SetUiIconsInTurnOrder(InDynamicAvatarTurnOrderImages);
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

			if (!IsReadyToStartMultiplayerBattle) {
				Server_SetReadyToStartMultiplayerBattle();
			}
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


// ------------------------- Battle
void APlayerController_Battle::Server_GetDataFromProfile_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Server_GetDataFromProfile / Server is attempting to get the player's data from their profile"));
	
	// ReSharper disable once CppLocalVariableMayBeConst
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	PlayerDataStruct = GameInstanceReference->PlayerData;

	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Server_GetDataFromProfile / PlayerName is: %s"), *PlayerDataStruct.PlayerName);
	UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Server_GetDataFromProfile / PlayerName has %d avatars"), PlayerDataStruct.CurrentAvatarTeam.Num());
}


void APlayerController_Battle::OnMouseCursorBeginHover(ACharacter_Pathfinder* ActingAvatar, TArray<FVector> PathBetweenAvatars)
{
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
						// Don't automatically end the turn just because someone attacked
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


void APlayerController_Battle::HighlightSpecificAvatarsAndTiles(TArray<ACharacter_Pathfinder*> Avatars, TArray<AActor_GridTile*> Tiles)
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


void APlayerController_Battle::BeginSelectingTileForReserveAvatar(bool DidAvatarDie)
{
	// Update the UI (which UI?)

	if (DidAvatarDie) {
		// Highlight each valid tile that the player can summon an avatar to
		TArray<AActor*> GridTilesArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);

		for (AActor* TileAsActor : GridTilesArray) {
			AActor_GridTile* Tile = Cast<AActor_GridTile>(TileAsActor);

			if (Tile) {
				if (Tile->AssignedMultiplayerUniqueID == MultiplayerUniqueID) {
					Tile->SetTileHighlightProperties(true, false, E_GridTile_ColourChangeContext::WithinAttackRange);
				} else {
					Tile->SetTileHighlightProperties(false, true, E_GridTile_ColourChangeContext::Normal);
				}
			}
		}
	} else {
		// Get the currently acting avatar to swap data
	}


	// Set the players' mouse mode to select a tile
	PlayerClickMode = E_PlayerCharacter_ClickModes::SummonAvatar;

	// Highlight the currently acting avatar if it isn't the end of the turn and an avatar was defeated
	// Otherwise, highlight each valid tile that the player can summon avatars to
	TArray<ACharacter_Pathfinder*> Avatars = { CurrentSelectedAvatar };
	TArray<AActor*> WorldGridArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_WorldGrid::StaticClass(), WorldGridArray);
	AActor_WorldGrid* WorldGridRef = Cast<AActor_WorldGrid>(WorldGridArray[0]);
	TArray<AActor_GridTile*> TileAtAvatarLocation { WorldGridRef->GetAndReturnGridTileAtLocation(WorldGridRef->ConvertGridTileLocationToCoordinates(CurrentSelectedAvatar->GetActorLocation())) };
	HighlightSpecificAvatarsAndTiles(Avatars, TileAtAvatarLocation);
}


void APlayerController_Battle::SummonReserveAvatarAtSelectedTile(AActor_GridTile* SelectedTile, ACharacter_Pathfinder* SelectedAvatar)
{
	// Tell the server to physically create the avatar actor, then end the turn
	// OR, swap the data of the pre-existing avatar actor with the reserve avatar

	FAvatar_Struct ReserveAvatarData = PlayerDataStruct.CurrentAvatarTeam[4];
	int ReserveAvatarIndexInParty = ReserveAvatarData.IndexInPlayerLibrary;
	ACharacter_Pathfinder* FoundAvatar;

	if (!SelectedAvatar) {
		TArray<AActor*> WorldGridArray;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_WorldGrid::StaticClass(), WorldGridArray);
		AActor_WorldGrid* FoundWorldGrid = Cast<AActor_WorldGrid>(WorldGridArray[0]);
		FIntPoint ActorCoordinates = FoundWorldGrid->ConvertGridTileLocationToCoordinates(Cast<AStarmark_GameState>(GetWorld()->GetGameState())->ReturnCurrentlyActingAvatar()->GetActorLocation());

		FoundAvatar = FoundWorldGrid->GetAndReturnCharacterAtLocation(ActorCoordinates);
	} else {
		FoundAvatar = SelectedAvatar;
	}

	// Swap indices
	ReserveAvatarData.IndexInPlayerLibrary = FoundAvatar->AvatarData.IndexInPlayerLibrary;
	FoundAvatar->AvatarData.IndexInPlayerLibrary = ReserveAvatarIndexInParty;
	FoundAvatar->AvatarData = ReserveAvatarData;
	
	// New avatar's attacks
	FoundAvatar->CurrentKnownAttacks.Empty();
	for (int i = 0; i < ReserveAvatarData.CurrentAttacks.Num(); i++) {
		FoundAvatar->CurrentKnownAttacks.Add(ReserveAvatarData.CurrentAttacks[i]);
	}

	// Set health and mana?

	PlayerClickMode = E_PlayerCharacter_ClickModes::E_MoveCharacter;
	Cast<AStarmark_GameState>(GetWorld()->GetGameState())->AvatarEndTurn();
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
	if (CurrentSelectedAvatar) {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateAttacksInHud_Implementation / Passed avatar: %s"), *ActingAvatar->AvatarData.Nickname);

		if (BattleWidgetReference) {
			UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateAttacksInHud_Implementation / Initializing HUD"));
			BattleWidgetReference->UpdateAvatarAttacksComponents(ActingAvatar->CurrentKnownAttacks);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateAttacksInHud_Implementation / Error: HUD reference is not valid"));
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_UpdateAttacksInHud_Implementation / CurrentSelectedAvatar is not valid"));
	}
}


void APlayerController_Battle::Client_ShowHideHud_Implementation(bool ShowHud)
{
	if (BattleWidgetReference) {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_ShowHideHud / Showing or Hiding HUD"));
		BattleWidgetReference->ShowHideActingPlayerHudElements(ShowHud);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Battle / Client_ShowHideHud / Error: HUD reference is not valid"));
	}
}


// ------------------------- Multiplayer Battle
void APlayerController_Battle::GetAvatarUpdateFromServer_Implementation(ACharacter_Pathfinder* AvatarReference, int AvatarUniqueID, bool IsCurrentlyActing, bool IsCurrentlSelectedAvatar)
{
	if (IsValid(AvatarReference)) {
		LocalAvatarUpdate(AvatarReference, AvatarUniqueID, IsCurrentlyActing, IsCurrentlSelectedAvatar);
	}
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
