#include "Starmark_GameState.h"


#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "PlayerController_Base.h"
#include "Starmark_PlayerState.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"


void AStarmark_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStarmark_GameState, AvatarTurnOrder);
}


void AStarmark_GameState::SetTurnOrder_Implementation(const TArray<APlayerController_Base*>& PlayerControllers)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundActors);

	// Use Nested Loops to compare Avatars' Speeds.
	for (int i = 0; i < FoundActors.Num(); i++) {
		//AvatarTurnOrder.Add(Cast<ACharacter_Pathfinder>(FoundActors[i]));

		if (AvatarTurnOrder.Num() <= 0) {
			AvatarTurnOrder.Add(Cast<ACharacter_Pathfinder>(FoundActors[i]));
		} else {
			for (int j = 0; j < AvatarTurnOrder.Num(); j++) {
				if (Cast<ACharacter_Pathfinder>(FoundActors[i])->AvatarData.BaseStats.Speed >= AvatarTurnOrder[j]->AvatarData.BaseStats.Speed && 
					!AvatarTurnOrder.Contains(Cast<ACharacter_Pathfinder>(FoundActors[i]))) {
					AvatarTurnOrder.Insert(Cast<ACharacter_Pathfinder>(FoundActors[i]), j);
					break;
				}
				// If we reach the end of the array and the Avatar isn't faster than any of the other Avatars, just add it at the end
				if (j == AvatarTurnOrder.Num() - 1 && !AvatarTurnOrder.Contains(Cast<ACharacter_Pathfinder>(FoundActors[i]))) {
					AvatarTurnOrder.Add(Cast<ACharacter_Pathfinder>(FoundActors[i]));
				}
			}
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Avatar Turn Order Length: %d"), AvatarTurnOrder.Num()));

	//CurrentAvatarTurnIndex = 0;

	// Set Player's Avatar to the first in the list
	//for (int i = 0; i < PlayerControllers.Num(); i++) {
	//	// Update All Avatar's Decals
	//	PlayerControllers[i]->UpdateSelectedAvatar();

	//	// Update the Player's HUD
	//	//if (PlayerControllers[i]->BattleHUDReference) {
	//	//	//BattleHUD_Reference = PlayerControllers[i]->BattleHUDReference;

	//	//	PlayerControllers[i]->BattleHUDReference->AvatarBattleDataWidget->LinkedAvatar = PlayerControllers[i]->CurrentSelectedAvatar;
	//	//	PlayerControllers[i]->BattleHUDReference->AvatarBattleDataWidget->SetAvatarData();
	//	//	//PlayerControllers[i]->BattleHUDReference->TurnOrderTextBlock->SetText(FText::FromString(FString::FromInt(AvatarTurnOrder.Num())));
	//	//}
	//	//else {
	//	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Battle HUD Reference Not Valid")));
	//	//}
	//}

	//PlayerControllersArray = PlayerControllers;

	//for (int i = 0; i < AvatarTurnOrder.Num(); i++) {
	for (int j = 0; j < PlayerControllers.Num(); j++) {
		if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerControllers[j]) {
			PlayerControllers[j]->IsCurrentlyActingPlayer = true;
			//PlayerControllers[j]->CurrentSelectedAvatar = AvatarTurnOrder[CurrentAvatarTurnIndex];
		} else {
			PlayerControllers[j]->IsCurrentlyActingPlayer = false;
		}

		/*AStarmark_PlayerState* PlayerState = Cast<AStarmark_PlayerState>(PlayerControllers[j]->PlayerState);

		if (PlayerControllers[j]->BattleHUDCodeReference->IsValidLowLevel())
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Battle HUD Valid")));
		else 
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Battle HUD Not Valid")));*/
		//PlayerControllers[j]->BattleHUDReference->TurnOrderTextBlock->SetText(FText::FromString(FString::FromInt(i)));
		//PlayerControllers[j]->BattleHUDReference->TurnOrderTextBlock->SetText(FText::FromString(PlayerControllers[j]->BattleHUDReference->TurnOrderTextBlock->GetText().ToString() + "\n" + AvatarTurnOrder[i]->AvatarData.AvatarName));
	}
	//}
}


void AStarmark_GameState::AvatarEndTurn_Implementation(const TArray<APlayerController_Base*>& PlayerControllers)
{
	//if (!PlayerControllerReference)
	//	PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// Reset Decals
	//PlayerControllerReference->UpdateSelectedAvatar();

	CurrentAvatarTurnIndex++;

	// Reset Round
	if (CurrentAvatarTurnIndex >= AvatarTurnOrder.Num()) {
		CurrentAvatarTurnIndex = 0;
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Avatars: %d  /  Turn Index: %d"), AvatarTurnOrder.Num(), CurrentAvatarTurnIndex));

	for (int j = 0; j < PlayerControllers.Num(); j++) {
		if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerControllers[j]) {
			PlayerControllers[j]->IsCurrentlyActingPlayer = true;
			//PlayerControllers[j]->CurrentSelectedAvatar = AvatarTurnOrder[CurrentAvatarTurnIndex];
		}
		else {
			PlayerControllers[j]->IsCurrentlyActingPlayer = false;
		}
	}

	//// Set Controlling Player
	//for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	//{
	//	APlayerController_Base* PlayerController = Cast<APlayerController_Base>(*Iterator);
	//	if (PlayerController)
	//	{
	//		if (PlayerController->IsValidLowLevel()) {
	//			if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerController) {
	//				PlayerController->IsCurrentlyActingPlayer = true;
	//				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("PlayerController Valid")));
	//			}
	//			else {
	//				PlayerController->IsCurrentlyActingPlayer = false;
	//				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("PlayerController Not Valid")));
	//			}
	//		}
	//	}
	//}

	//PlayerControllerReference->CurrentSelectedAvatar = Cast<ACharacter_Pathfinder>(AvatarTurnOrder[CurrentAvatarTurnIndex]);

	//AvatarBeginTurn();
}


void AStarmark_GameState::AvatarBeginTurn()
{
	ACharacter_Pathfinder* AvatarRef = PlayerControllerReference->CurrentSelectedAvatar;

	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// Reset Decals
	PlayerControllerReference->UpdateSelectedAvatar();

	// Reset Moves
	AvatarRef->CurrentTileMoves = AvatarRef->AvatarData.MaximumTileMoves;

	// Apply Status Effects
	for (int i = AvatarRef->CurrentStatusEffectsArray.Num() - 1; i == 0; i--) {
		AvatarRef->CurrentStatusEffectsArray[i].TurnsRemaining--;

		if (!AvatarRef->CurrentStatusEffectsArray[i].TurnsRemaining == 0) {
			// Switch On Status Effect Name
			if (AvatarRef->CurrentStatusEffectsArray[i].Name == "Paralyzed") {
				AvatarRef->CurrentTileMoves = FMath::FloorToInt(AvatarRef->CurrentTileMoves / 2);
			}
		} else {
			AvatarRef->CurrentStatusEffectsArray.RemoveAt(i);
		}
	}

	// Re-Calculate Stats

	// Update HUD
	//if (BattleHUD_Reference) {
		//BattleHUD_Reference->AvatarBattleDataWidget->LinkedAvatar = AvatarRef->AvatarData;
		//BattleHUD_Reference->AvatarBattleDataWidget->SetAvatarData();
	//}

	//for (int j = 0; j < PlayerArray.Num(); j++) {
	//	if (AvatarTurnOrder[CurrentAvatarTurnIndex]->PlayerControllerReference == PlayerControllers[j]) {
	//		PlayerControllers[j]->IsCurrentlyActingPlayer = true;
	//		//PlayerControllers[j]->CurrentSelectedAvatar = AvatarTurnOrder[CurrentAvatarTurnIndex];
	//	}
	//	else {
	//		PlayerControllers[j]->IsCurrentlyActingPlayer = false;
	//	}
	//}
}


void AStarmark_GameState::GameState_LaunchAttack_Implementation(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	//int CurrentDamage = 1;

	//CurrentDamage = FMath::CeilToInt(Attacker->AvatarData.BaseStats.Attack * 50);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 1 = %d"), CurrentDamage));
	//CurrentDamage = FMath::CeilToInt(CurrentDamage / Defender->AvatarData.BaseStats.Defence);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 2 = %d"), CurrentDamage));
	//CurrentDamage = FMath::CeilToInt((Attacker->AvatarData.BaseStats.Power / 2) * CurrentDamage);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 3 = %d"), CurrentDamage));
	//CurrentDamage = FMath::CeilToInt(CurrentDamage / 8);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 3 = %d"), CurrentDamage));

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Final Damage  = %d"), CurrentDamage));

	//if (Defender) {
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Defender Valid")));
	//	Defender->AvatarData.CurrentHealthPoints -= CurrentDamage;
	//	Defender->UpdatePlayerBattleHUD();
	//}
	//else {
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Defender Not Valid")));
	//}
	if (this->HasAuthority())
		Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->GameMode_LaunchAttack_Implementation(Attacker, Defender);
}