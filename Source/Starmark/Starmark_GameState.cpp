#include "Starmark_GameState.h"

#include "Actor_AbilitiesLibrary.h"
#include "Actor_GridTile.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character_NonAvatarEntity.h"
#include "Character_Pathfinder.h"
#include "Engine/World.h"
#include "Starmark_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController_Battle.h"
#include "Widget_HUD_Battle.h"


void AStarmark_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStarmark_GameState, AvatarTurnOrder);
	DOREPLIFETIME(AStarmark_GameState, DynamicAvatarTurnOrder);
	DOREPLIFETIME(AStarmark_GameState, DynamicAvatarTurnOrderImages);
	DOREPLIFETIME(AStarmark_GameState, CurrentTurnOrderText);
}


// ------------------------- Local Helper Functions


// ------------------------- Global Helper Functions
ACharacter_Pathfinder* AStarmark_GameState::ReturnCurrentlyActingAvatar()
{
	if (DynamicAvatarTurnOrder.Num() > 0) {
		CurrentlyActingAvatar = DynamicAvatarTurnOrder[0];
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / ReturnCurrentlyActingAvatar / Currently acting avatar: %s"), *CurrentlyActingAvatar->AvatarData.Nickname);
	}

	return CurrentlyActingAvatar;
}


APlayerController_Battle* AStarmark_GameState::ReturnCurrentlyActingPlayer()
{
	CurrentlyActingPlayer = DynamicAvatarTurnOrder[0]->PlayerControllerReference;
	CurrentlyActingPlayer->IsCurrentlyActingPlayer = true;
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / ReturnCurrentlyActingPlayer / Currently acting player: %s"), *CurrentlyActingPlayer->PlayerDataStruct.PlayerName);

	return CurrentlyActingPlayer;
}


void AStarmark_GameState::ShowHideAllPlayerHuds()
{
	TArray<AActor*> PlayerControllerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_Battle::StaticClass(), PlayerControllerActors);

	for (AActor* ControllerActor : PlayerControllerActors) {
		APlayerController_Battle* Controller = Cast<APlayerController_Battle>(ControllerActor);
		
		if (Controller == CurrentlyActingPlayer) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / ShowHideAllPlayerHuds / Show HUD for player: %s"), *Controller->PlayerDataStruct.PlayerName);
			Controller->Client_ShowHideHud(true);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / ShowHideAllPlayerHuds / Hide HUD for player: %s"), *Controller->PlayerDataStruct.PlayerName);
			Controller->Client_ShowHideHud(false);
		}
	}
}


FPlayer_Data AStarmark_GameState::FindPlayerDataUsingMultiplayerUniqueID(int MultiplayerUniqueID)
{
	// Apparently this is how C++ wants you to initialize pass-by-reference variables.
	//FPlayer_Data& DefaultReturnValue = PlayerDataStructsArray[0];
	
	for (auto& Element : PlayerDataAndUniqueIDMap) {
		if (Element.Key == MultiplayerUniqueID) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / FindPlayerDataUsingMultiplayerUniqueID / Returning PlayerData at index %d with name: %s."), MultiplayerUniqueID, *Element.Value.PlayerName);
			return Element.Value;
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / FindPlayerDataUsingMultiplayerUniqueID / Found PlayerData at index %d with name: %s."), Element.Key, *Element.Value.PlayerName);
		}
	}

	// Return the default data if the player data can't be found.
	return FPlayer_Data();
}


FPlayer_Data& AStarmark_GameState::PassByReferencePlayerDataUsingMultiplayerUniqueID(int MultiplayerUniqueID)
{
	for (auto& Element : PlayerDataAndUniqueIDMap) {
		if (Element.Key == MultiplayerUniqueID) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / FindPlayerDataUsingMultiplayerUniqueID / Returning PlayerData at index %d with name: %s."), MultiplayerUniqueID, *Element.Value.PlayerName);
			return Element.Value;
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / FindPlayerDataUsingMultiplayerUniqueID / Found PlayerData at index %d with name: %s."), Element.Key, *Element.Value.PlayerName);
		}
	}

	// Return the default data if the player data can't be found.
	return PlayerDataAndUniqueIDMap[0];
}


FAvatar_Struct& AStarmark_GameState::FindAvatarUsingUniqueID(int AvatarUniqueID)
{
	FAvatar_Struct& DefaultReturnValue = AvatarTurnOrder[0]->AvatarData;
	
	for (auto& Element : PlayerDataAndUniqueIDMap) {
		for (FAvatar_Struct Avatar : Element.Value.CurrentAvatarTeam) {
			if (Avatar.BattleUniqueID == AvatarUniqueID) {
				DefaultReturnValue = Avatar;
				break;
			}
		}
	}
	
	return DefaultReturnValue;
}


void AStarmark_GameState::AppendStringToCombatLog_Implementation(const FString& AppendString)
{
	CombatLogString.Append(AppendString + "\n");
	
	TArray<AActor*> PlayerControllerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_Battle::StaticClass(), PlayerControllerActors);

	for (int i = 0; i < PlayerControllerActors.Num(); i++) {
		Cast<APlayerController_Battle>(PlayerControllerActors[i])->GetUpdatedCombatLogTextFromGameState(CombatLogString);
	}
}


// ------------------------- Battle
void AStarmark_GameState::SetTurnOrder_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / SetTurnOrder / Begin function"));
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundActors);
	TArray<ACharacter_Pathfinder*> SlowedAvatars, SlowedAvatarsInTurnOrder;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / SetTurnOrder / Empty the AvatarTurnOrder array"));
	AvatarTurnOrder.Empty();

	// Use Nested Loops to compare Avatars' Speeds.
	for (int i = 0; i < FoundActors.Num(); i++) {
		ACharacter_Pathfinder* CurrentAvatar = Cast<ACharacter_Pathfinder>(FoundActors[i]);
		if (!Cast<ACharacter_NonAvatarEntity>(CurrentAvatar)) {
			for (int j = 0; j < CurrentAvatar->CurrentStatusEffectsArray.Num(); j++) {
				if (CurrentAvatar->CurrentStatusEffectsArray[j].Name == "Drowning") {
					SlowedAvatars.AddUnique(CurrentAvatar);
					break;
				}
			}

			if (!SlowedAvatars.Contains(CurrentAvatar)) {
				if (AvatarTurnOrder.Num() <= 0)
					AvatarTurnOrder.Add(CurrentAvatar);
				else {
					for (int j = 0; j < AvatarTurnOrder.Num(); j++) {
						ACharacter_Pathfinder* AvatarInTurnOrder = Cast<ACharacter_Pathfinder>(AvatarTurnOrder[j]);

						if (CurrentAvatar->AvatarData.BattleStats.Speed >= AvatarInTurnOrder->AvatarData.BattleStats.Speed &&
							!AvatarTurnOrder.Contains(CurrentAvatar)) {
							AvatarTurnOrder.Insert(CurrentAvatar, j);
							break;
						}

						// If we reach the end of the array and the Avatar isn't faster than any of the other Avatars, just add it at the end
						if (j == AvatarTurnOrder.Num() - 1 && !AvatarTurnOrder.Contains(CurrentAvatar))
							AvatarTurnOrder.Add(CurrentAvatar);
					}
				}
			}
		}
	}

	// Compare slowed Actors' speed
	for (int i = 0; i < SlowedAvatars.Num(); i++) {
		ACharacter_Pathfinder* CurrentAvatar = Cast<ACharacter_Pathfinder>(SlowedAvatars[i]);

		if (!Cast<ACharacter_NonAvatarEntity>(CurrentAvatar)) {
			if (SlowedAvatarsInTurnOrder.Num() <= 0) {
				SlowedAvatarsInTurnOrder.Add(CurrentAvatar);
			} else {
				for (int j = 0; j < SlowedAvatarsInTurnOrder.Num(); j++) {
					ACharacter_Pathfinder* AvatarInTurnOrder = Cast<ACharacter_Pathfinder>(SlowedAvatarsInTurnOrder[j]);

					if (CurrentAvatar->AvatarData.BattleStats.Speed >= AvatarInTurnOrder->AvatarData.BattleStats.Speed &&
						!SlowedAvatarsInTurnOrder.Contains(CurrentAvatar)) {
						SlowedAvatarsInTurnOrder.Insert(CurrentAvatar, j);
						break;
					}

					// If we reach the end of the array and the Avatar isn't faster than any of the other Avatars, just add it at the end
					if (j == AvatarTurnOrder.Num() - 1 && !SlowedAvatarsInTurnOrder.Contains(CurrentAvatar))
						SlowedAvatarsInTurnOrder.Add(CurrentAvatar);
				}
			}
		}
	}

	for (int i = 0; i < SlowedAvatarsInTurnOrder.Num(); i++) {
		AvatarTurnOrder.Add(SlowedAvatarsInTurnOrder[i]);
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / SetTurnOrder / End function"));
}


void AStarmark_GameState::OnRepNotify_DynamicAvatarTurnOrderUpdated_Implementation()
{
	// To-Do: Update this function with a better name

	// Tell each player controller to update their HUDs.
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Begin function"));
	TArray<APlayerController_Battle*> ActorsArray = Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->GetAllBattlePlayerControllers();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / This function is being multicast to all clients."));
	DynamicAvatarTurnOrderImages.Empty();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Fill the DynamicAvatarTurnOrderImages array."));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / DynamicAvatarTurnOrderImages array length: %d"), DynamicAvatarTurnOrder.Num());
	// Put together an array of the avatars' images in order.
	for (int i = 0; i < DynamicAvatarTurnOrder.Num(); i++) {
		if (DynamicAvatarTurnOrder.IsValidIndex(i)) {
			// To-Do: Fix this then un-comment it out
			//UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Add avatar %s's image to the DynamicAvatarTurnOrderImages array at index %d"), *DynamicAvatarTurnOrder[i]->AvatarData.Nickname, i);
			if (DynamicAvatarTurnOrder[i]->AvatarData.UiImages.IsValidIndex(0)) {
				DynamicAvatarTurnOrderImages.Add(DynamicAvatarTurnOrder[i]->AvatarData.UiImages[0]);
			} else {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Avatar does not have a valid UiImage?"));
			}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / DynamicAvatarTurnOrde index %d is not valid."), i);
		}
	}
	
	for (int i = 0; i < ActorsArray.Num(); i++) {
		if (ActorsArray.IsValidIndex(i)) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Set UI Icons for the player controller at index %d."), i);

			if (!ActorsArray[i]->BattleWidgetReference) {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / This player doesn't have a valid BattleWidgetReference. Attempting to find one..."));

				TArray<UUserWidget*> FoundBattleHudWidgets;
				UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundBattleHudWidgets, UWidget_HUD_Battle::StaticClass(), true);

				if (FoundBattleHudWidgets.Num() > 0) {
					ActorsArray[i]->BattleWidgetReference = Cast<UWidget_HUD_Battle>(FoundBattleHudWidgets[0]);

					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Found a battle widget"));
				} else {
					ActorsArray[i]->CreateBattleWidget();
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Could not find a battle widget!"));
				}
			}
			
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Sending %d UI Icons."), DynamicAvatarTurnOrderImages.Num());
			ActorsArray[i]->BattleWidgetReference->SetUiIconsInTurnOrder(DynamicAvatarTurnOrderImages);

			// Also update the current acting avatar in the hud
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Update the currently acting entity information."));
			ActorsArray[i]->BattleWidgetReference->SetCurrentActingEntityInfo(DynamicAvatarTurnOrder[0]);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / End function"));
}


void AStarmark_GameState::AvatarBeginTurn_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarBeginTurn / Begin function"));

	/*
	if (AvatarTurnOrder.IsValidIndex(CurrentAvatarTurnIndex)) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarBeginTurn / Begin a new turn"));

		ACharacter_Pathfinder* Avatar = AvatarTurnOrder[CurrentAvatarTurnIndex];
		Avatar->AvatarData.CurrentTileMoves = AvatarTurnOrder[CurrentAvatarTurnIndex]->AvatarData.MaximumTileMoves;

		// Assign helper variables
		CurrentlyActingAvatar = AvatarTurnOrder[CurrentAvatarTurnIndex];
		CurrentlyActingPlayer = Cast<APlayerController_Battle>(CurrentlyActingAvatar->PlayerControllerReference);

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarBeginTurn / Currently acting avatar: %s"), *CurrentlyActingAvatar->AvatarData.Nickname);
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarBeginTurn / Currently acting player: %s"), *CurrentlyActingPlayer->PlayerDataStruct.PlayerName);

		// Reduce durations of all statuses
		//for (int i = Avatar->CurrentStatusEffectsArray.Num() - 1; i >= 0; i--) {
		//	Avatar->CurrentStatusEffectsArray[i].TurnsRemaining--;

		//	if (Avatar->CurrentStatusEffectsArray[i].TurnsRemaining <= 0) {
		//		if (IsValid(Avatar->CurrentStatusEffectsArray[i].SpecialFunctionsActor)) {
		//			Avatar->CurrentStatusEffectsArray[i].SpecialFunctionsActor->OnStatusEffectRemoved(Avatar, Avatar->CurrentStatusEffectsArray[i]);
		//		} else
		//			Avatar->CurrentStatusEffectsArray.RemoveAt(i);
		//	} else {
		//		// On Status Effect Start-of-turn effects
		//		if (Avatar->CurrentStatusEffectsArray[i].Name == "Stunned") {
		//			// Do nothing yet
		//		} else if (IsValid(Avatar->CurrentStatusEffectsArray[i].SpecialFunctionsActor)) {
		//			Avatar->CurrentStatusEffectsArray[i].SpecialFunctionsActor->OnStatusEffectStartOfTurn(Avatar, Avatar->CurrentStatusEffectsArray[i]);
		//		}
		//	}
		//}

		// Check for any abilities that trigger at the start of the turn
		/*
		if (Avatar->AvatarData.Ability.TriggerCondition == E_Ability_TriggerConditions::OnAvatarStartOfTurn) {
			// Ensure that there is only one AbilityActor because there only needs to be one that all Avatars can use
			if (Avatar->AvatarData.Ability.AbilityLibraryActor == nullptr) {
				TArray<AActor*> AbilityLibraryActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_AbilitiesLibrary::StaticClass(), AbilityLibraryActors);

				if (AbilityLibraryActors.Num() > 0) {
					for (int i = 0; i < AbilityLibraryActors.Num(); i++) {
						if (i == 0) 
							Avatar->AvatarData.Ability.AbilityLibraryActor = Cast<AActor_AbilitiesLibrary>(AbilityLibraryActors[i]);
						else
							AbilityLibraryActors[i]->Destroy();
					}
				} else {
					const FActorSpawnParameters SpawnInfo;
					Avatar->AvatarData.Ability.AbilityLibraryActor = GetWorld()->SpawnActor<AActor_AbilitiesLibrary>(AActor_AbilitiesLibrary::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);
				}
			}

			// Call the ability function
			Avatar->AvatarData.Ability.AbilityLibraryActor->SwitchOnAbilityEffect(Avatar->AvatarData.Ability.Function, Avatar, Avatar);
		}

		// Check that the currently acting entity isn't stunned
		if (StunStatus.Name != "Stunned") {
			StunStatus = *StatusEffectsDataTable->FindRow<FAvatar_StatusEffect>("Stunned", GameStateContextString);
		}

		if (!Avatar->CurrentStatusEffectsArray.Contains(StunStatus)) {
			// If the currently acting entity is an enemy, activate their AI functions.
			if (Avatar->AvatarData.Factions.Contains(EEntity_Factions::Enemy1)) {
				AAIController_EnemyEntity* EnemyController = Cast<AAIController_EnemyEntity>(Avatar->GetController());

				if (EnemyController->SelfEntityReference != Avatar) {
					EnemyController->SelfEntityReference = Avatar;
					EnemyController->Possess(Avatar);
				}

				EnemyController->StepOne_ChooseTarget();
			}
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarBeginTurn / CurrentAvatarTurnIndex isn't valid?"));
	}
	*/

	// All players' HUDs should be checked at the end of every turn, not just their own
	/*
	// Reset the players' hud
	TArray<UUserWidget*> FoundBattleHudWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundBattleHudWidgets, UWidget_HUD_Battle::StaticClass(), true);
	for (UUserWidget* FoundWidget : FoundBattleHudWidgets) {
		UWidget_HUD_Battle* HUD = Cast<UWidget_HUD_Battle>(FoundWidget);

		// If the player is acting, reset their HUD
		// otherwise, hide most of their hud

		
		HUD->ResetBattleHud();
	}
	*/

	if (GameModeReference == nullptr) {
		GameModeReference = Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode());
	}

	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarBeginTurn / This game state has authority"));

		GameModeReference->Server_AvatarBeginTurn(CurrentAvatarTurnIndex);
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarBeginTurn / End function"));
}


void AStarmark_GameState::AvatarEndTurn_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Begin function"));
	
	TArray<bool> IsPlayerActingArray;
	TArray<AActor*> GridTilesArray, AvatarsActorsArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), AvatarsActorsArray);
	for (int i = 0; i < AvatarsActorsArray.Num(); i++) {
		ACharacter_Pathfinder* FoundAvatar = Cast<ACharacter_Pathfinder>(AvatarsActorsArray[i]);

		if (FoundAvatar) {
			FoundAvatar->SynchronizeAvatarDataToGameState();
		}
	}

	CurrentAvatarTurnIndex++;

	// Reset Round if all Avatars have acted
	if (CurrentAvatarTurnIndex >= AvatarTurnOrder.Num()) {
		CurrentAvatarTurnIndex = 0;
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / CurrentAvatarTurnIndex is: %d"), CurrentAvatarTurnIndex);

	// To-Do: Move this functionality to the GameMode, since it doesn't work here
	/*
	for (int j = 0; j < PlayerArray.Num(); j++) {
		APlayerController_Battle* PlayerController = Cast<APlayerController_Battle>(PlayerArray[j]->GetPawn()->GetController());

		if (PlayerController) {
			if (AvatarTurnOrder.IsValidIndex(CurrentAvatarTurnIndex)) {
				if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerController) {
					// Check that the currently acting entity isn't stunned (?)

					PlayerController->IsCurrentlyActingPlayer = true;
					PlayerController->CurrentSelectedAvatar = AvatarTurnOrder[CurrentAvatarTurnIndex];

					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / It's player %s's turn"), *PlayerController->PlayerData.PlayerName);
				} else {
					PlayerController->IsCurrentlyActingPlayer = false;
					PlayerController->CurrentSelectedAvatar = nullptr;

					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Player %s is not acting now"), *PlayerController->PlayerData.PlayerName);
				}
			} else {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Avatar in turn order at index %d is not valid"), CurrentAvatarTurnIndex);
			}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Player controller is not valid"));
		}
	}
	*/

	// Set all GridTiles to default colours and visibility
	TArray<ACharacter_Pathfinder*> Avatars;
	TArray<AActor_GridTile*> Tiles;
	
	TArray<AActor*> PlayerControllerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_Battle::StaticClass(), PlayerControllerActors);
	for (int i = 0; i < PlayerControllerActors.Num(); i++) {
		Cast<APlayerController_Battle>(PlayerControllerActors[i])->HighlightSpecificAvatarsAndTiles(Avatars, Tiles);
	}

	// Update the dynamic turn order
	if (DynamicAvatarTurnOrder.Num() > 0) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Update the DynamicAvatarTurnOrder array"));
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Some avatars in the DynamicAvatarTurnOrder array"));
		
		DynamicAvatarTurnOrder.RemoveAt(0);
		//DynamicAvatarTurnOrder.Add(FirstAvatarInDynamicTurnOrder);

		if (DynamicAvatarTurnOrder.Num() < 1) {
			DynamicAvatarTurnOrder = AvatarTurnOrder;

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Every avatar has had a turn. Start a new round here."));
		} else {
			//UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Avatar %s is up next"), *DynamicAvatarTurnOrder[0]->AvatarData.Nickname);
		}

		// Update the helper variables here
		//CurrentlyActingAvatar = DynamicAvatarTurnOrder[0];
		//CurrentlyActingPlayer = CurrentlyActingAvatar->PlayerControllerReference;

		// Assign currently controlled avatars based on the dynamic turn order
		for (int i = DynamicAvatarTurnOrder.Num() - 1; i >= 0; i--) {
			if (IsValid(DynamicAvatarTurnOrder[i])) {
				if (DynamicAvatarTurnOrder[i]->PlayerControllerReference->IsValidLowLevel()) {

					for (int x = DynamicAvatarTurnOrder[i]->CurrentStatusEffectsArray.Num() - 1; x >= 0; x--) {
						if (DynamicAvatarTurnOrder[i]->CurrentStatusEffectsArray[x].Name == "Stunned") {
							GetWorldTimerManager().SetTimer(StunTimerHandle, this, &AStarmark_GameState::StunDelayedSkipTurn, 1.f);
							DynamicAvatarTurnOrder[i]->CurrentStatusEffectsArray.RemoveAt(x);
						}
					}

					// Clean up entities' controllers
					DynamicAvatarTurnOrder[i]->PlayerControllerReference->TileHighlightMode = E_PlayerCharacter_HighlightModes::E_MovePath;
				} else {
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Next entity in turn order does not have a player controller."));
				}

				// Clean up all entities
				DynamicAvatarTurnOrder[i]->CurrentSelectedAttack.Name = "None";
				DynamicAvatarTurnOrder[i]->CurrentSelectedAttack.AttachAttackTraceActorToMouse = false;
				DynamicAvatarTurnOrder[i]->ValidAttackTargetsArray.Empty();
				DynamicAvatarTurnOrder[i]->AttackTraceActor->SetVisibility(false);
				DynamicAvatarTurnOrder[i]->AttackTraceActor->SetHiddenInGame(true);
			}
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / No avatars in the DynamicAvatarTurnOrder array"))
	}

	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Server_UpdateAllAvatarDecals();

	AvatarBeginTurn();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / End function"));
}


void AStarmark_GameState::EndOfBattle_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / EndOfBattle / Begin function"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / EndOfBattle / Return each player to the Main Menu"));
	
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("End Of Battle. Returning to Main Menu...")));

	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->EndOfBattle();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / EndOfBattle / End function"));
}


void AStarmark_GameState::StunDelayedSkipTurn_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / StunDelayedSkipTurn / Begin function"));
	
	AvatarEndTurn();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / StunDelayedSkipTurn / End function"));
}