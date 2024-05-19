#include "Starmark_GameState.h"

#include "Actor_AbilitiesLibrary.h"
#include "Actor_GridTile.h"
#include "Actor_StatusEffectsLibrary.h"
#include "AIController_EnemyEntity.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character_NonAvatarEntity.h"
#include "Character_Pathfinder.h"
#include "Engine/World.h"
#include "Starmark_PlayerState.h"
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
	CurrentlyActingPlayer = AvatarTurnOrder[0]->PlayerControllerReference;
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / ReturnCurrentlyActingPlayer / Currently acting player: %s"), *CurrentlyActingPlayer->PlayerDataStruct.PlayerName);

	return CurrentlyActingPlayer;
}


TArray<APlayerController_Battle*> AStarmark_GameState::ReturnAllBattlePlayerControllers()
{
	TArray<AActor*> ActorsArray;
	TArray<APlayerController_Battle*> ControllersArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_Battle::StaticClass(), ActorsArray);

	for (AActor* Actor : ActorsArray) {
		if (Cast<APlayerController_Battle>(Actor)) {
			ControllersArray.Add(Cast<APlayerController_Battle>(Actor));
		}
	}

	return ControllersArray;
}


void AStarmark_GameState::ShowHideAllPlayerHuds()
{
	ReturnCurrentlyActingPlayer();

	for (auto Controller : ReturnAllBattlePlayerControllers()) {
		if (Controller == CurrentlyActingPlayer) {
			Controller->Client_ShowHideHud(true);
		} else {
			Controller->Client_ShowHideHud(false);
		}
	}
}


// ------------------------- Battle
void AStarmark_GameState::SetTurnOrder_Implementation(const TArray<APlayerController_Battle*>& PlayerControllers)
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

	// To-Do: Shift the avatars around in the dynamic array, so that they are in dynamic order
	// and the first avatar is the acting one

	//OnRepNotify_DynamicAvatarTurnOrderUpdated();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / SetTurnOrder / End function"));
}


void AStarmark_GameState::OnRepNotify_DynamicAvatarTurnOrderUpdated()
{
	// To-Do: Update this function with a better name
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Begin function"));
	
	// Tell each player controller to update their HUDs
	TArray<AActor*> ActorsArray;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / This function is being multicast to all clients."));

	DynamicAvatarTurnOrderImages.Empty();

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Fill the DynamicAvatarTurnOrderImages array"));
	// Put together a TArray of the avatar's images in order
	for (int i = 0; i < DynamicAvatarTurnOrder.Num(); i++) {
		if (DynamicAvatarTurnOrder.IsValidIndex(i)) {
			// To-Do: Fix this then un-comment it out
			//UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Add avatar %s's image to the DynamicAvatarTurnOrderImages array at index %d"), *DynamicAvatarTurnOrder[i]->AvatarData.Nickname, i);
			DynamicAvatarTurnOrderImages.Add(DynamicAvatarTurnOrder[i]->AvatarData.UiImages[0]);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated /DynamicAvatarTurnOrde index %d is not valid"), i);
		}
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_Battle::StaticClass(), ActorsArray);
	for (int i = 0; i < ActorsArray.Num(); i++) {
		if (Cast<APlayerController_Battle>(ActorsArray[i])) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Set UI Icons for the player controller at index %d"), i);

			if (!Cast<APlayerController_Battle>(ActorsArray[i])->BattleWidgetReference) {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / This player doesn't have a valid BattleWidgetReference. Attempting to find one..."));

				TArray<UUserWidget*> FoundBattleHudWidgets;
				UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundBattleHudWidgets, UWidget_HUD_Battle::StaticClass(), true);

				if (FoundBattleHudWidgets.Num() > 0) {
					Cast<APlayerController_Battle>(ActorsArray[i])->BattleWidgetReference = Cast<UWidget_HUD_Battle>(FoundBattleHudWidgets[0]);

					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Found a battle widget"));
				} else {
					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Could not find a battle widget!"));
				}
			}

			if (Cast<APlayerController_Battle>(ActorsArray[i])->BattleWidgetReference) {
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Sending %d UI Icons"), DynamicAvatarTurnOrderImages.Num());
				Cast<APlayerController_Battle>(ActorsArray[i])->BattleWidgetReference->SetUiIconsInTurnOrder(DynamicAvatarTurnOrderImages);

				// Also update the current acting avatar in the hud
				UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / Update the currently acting entity information."));
				Cast<APlayerController_Battle>(ActorsArray[i])->BattleWidgetReference->SetCurrentActingEntityInfo(DynamicAvatarTurnOrder[0]);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / OnRepNotify_DynamicAvatarTurnOrderUpdated / End function"));
}


void AStarmark_GameState::AvatarBeginTurn_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarBeginTurn / Begin function"));
	
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
		*/

		// Check that the currently acting entity isn't stunned
		if (StunStatus.Name != "Stunned") {
			StunStatus = *StatusEffectsDataTable->FindRow<FAvatar_StatusEffect>("Stunned", GameStateContextString);
		}

		if (!Avatar->CurrentStatusEffectsArray.Contains(StunStatus)) {
			// If the currently acting entity is an enemy, activate their AI functions
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
	
	TArray<ACharacter_Pathfinder*> AvatarArray;
	TArray<bool> IsPlayerActingArray;
	TArray<AActor*> GridTilesArray;

	CurrentAvatarTurnIndex++;

	// Check if an Avatar died this turn
	// If true, check for reserve Avatars before ending the turn

	// Reset Round if all Avatars have acted
	if (CurrentAvatarTurnIndex >= AvatarTurnOrder.Num()) {
		CurrentAvatarTurnIndex = 0;
	}

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / CurrentAvatarTurnIndex is: %d"), CurrentAvatarTurnIndex);

	// To-Do: Move this functionality somewhere else, since it doesn't work here
	/*
	for (int j = 0; j < PlayerArray.Num(); j++) {
		APlayerController_Battle* PlayerController = Cast<APlayerController_Battle>(PlayerArray[j]->GetPawn()->GetController());

		if (PlayerController) {
			if (AvatarTurnOrder.IsValidIndex(CurrentAvatarTurnIndex)) {
				if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerController) {
					// Check that the currently acting entity isn't stunned (?)

					PlayerController->IsCurrentlyActingPlayer = true;
					PlayerController->CurrentSelectedAvatar = AvatarTurnOrder[CurrentAvatarTurnIndex];

					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / It's player %s's turn"), *PlayerController->PlayerDataStruct.PlayerName);
				} else {
					PlayerController->IsCurrentlyActingPlayer = false;
					PlayerController->CurrentSelectedAvatar = nullptr;

					UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Player %s is not acting now"), *PlayerController->PlayerDataStruct.PlayerName);
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
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Reset all tile highlights"));
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	for (int j = 0; j < GridTilesArray.Num(); j++) {
		Cast<AActor_GridTile>(GridTilesArray[j])->SetTileHighlightProperties(false, true, E_GridTile_ColourChangeContext::Normal);
	}

	// Update the dynamic turn order
	if (DynamicAvatarTurnOrder.Num() > 0) {
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Update the DynamicAvatarTurnOrder array"));
		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Some avatars in the DynamicAvatarTurnOrder array"));

		ACharacter_Pathfinder* FirstAvatarInDynamicTurnOrder = DynamicAvatarTurnOrder[0];
		DynamicAvatarTurnOrder.RemoveAt(0);
		//DynamicAvatarTurnOrder.Add(FirstAvatarInDynamicTurnOrder);

		if (DynamicAvatarTurnOrder.Num() < 1) {
			DynamicAvatarTurnOrder = AvatarTurnOrder;

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Every avatar has had a turn. Start a new round here."));
		}

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameState / AvatarEndTurn / Avatar %s is up next"), *DynamicAvatarTurnOrder[0]->AvatarData.Nickname);

		// Update the helper variables here
		CurrentlyActingAvatar = DynamicAvatarTurnOrder[0];
		CurrentlyActingPlayer = CurrentlyActingAvatar->PlayerControllerReference;

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