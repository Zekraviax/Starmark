#include "Starmark_GameState.h"


#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "Widget_HUD_Battle.h"
#include "PlayerController_Base.h"


void AStarmark_GameState::SetTurnOrder(UWidget_HUD_Battle* Battle_HUD)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), FoundActors);

	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	for (int i = 0; i < FoundActors.Num(); i++) {
		//AvatarTurnOrder.Add(Cast<ACharacter_Pathfinder>(FoundActors[i]));
		AvatarTurnOrder.Insert(Cast<ACharacter_Pathfinder>(FoundActors[i]), i);
	}

	// Set Player's Avatar to the first in the list
	PlayerControllerReference->CurrentSelectedAvatar = Cast<ACharacter_Pathfinder>(AvatarTurnOrder[0]);
	CurrentAvatarTurnIndex = 0;

	// Update the Player's HUD
	if (Battle_HUD) {
		BattleHUD_Reference = Battle_HUD;

		for (int i = 0; i < AvatarTurnOrder.Num(); i++) {
			BattleHUD_Reference->TurnOrderTextBlock->SetText(FText::FromString(BattleHUD_Reference->TurnOrderTextBlock->GetText().ToString() + "\n" + AvatarTurnOrder[i]->AvatarData.AvatarName));
		}
	}

	// Update All Avatar's Decals
	PlayerControllerReference->UpdateSelectedAvatar();
}


void AStarmark_GameState::AvatarEndTurn()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	CurrentAvatarTurnIndex++;

	// Reset Round
	if (CurrentAvatarTurnIndex > AvatarTurnOrder.Num()) {
		CurrentAvatarTurnIndex = 0;
	}

	PlayerControllerReference->CurrentSelectedAvatar = Cast<ACharacter_Pathfinder>(AvatarTurnOrder[CurrentAvatarTurnIndex]);
}
