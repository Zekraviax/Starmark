#include "Starmark_GameMode.h"

#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "Starmark_GameState.h"


// ------------------------- Battle
void AStarmark_GameMode::GameMode_SetTurnOrder_Implementation()
{

}


void AStarmark_GameMode::Battle_SubtractHealth_Implementation(ACharacter_Pathfinder* Defender, int DamageDealt)
{
	if (Defender) {
		Defender->AvatarData.CurrentHealthPoints -= DamageDealt;
		Defender->UpdatePlayerParty();

		if (Defender->AvatarData.CurrentHealthPoints <= 0) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Avatar Defeated")));

			Battle_AvatarDefeated(Defender);
		}
	}
}


void AStarmark_GameMode::Battle_AvatarDefeated(ACharacter_Pathfinder* Avatar)
{
	Avatar->PlayerControllerReference->PlayerParty.RemoveAt(Avatar->IndexInPlayerParty);

	if (Avatar->PlayerControllerReference->PlayerParty.Num() <= 0) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player has run out of Avatars")));
		Cast<AStarmark_GameState>(GetWorld()->GetGameState())->EndOfBattle_Implementation();
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player Avatars Remaining: %d"), Avatar->PlayerControllerReference->PlayerParty.Num()));
	}

	Avatar->Destroy();
}


void AStarmark_GameMode::EndOfTurn_Implementation()
{
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	if (GameStateReference) {
		GameStateReference->CurrentAvatarTurnIndex++;

		if (GameStateReference->CurrentAvatarTurnIndex >= GameStateReference->AvatarTurnOrder.Num()) {
			GameStateReference->CurrentAvatarTurnIndex = 0;
		}

		for (int i = 0; i < GameStateReference->PlayerArray.Num(); i++) {

		}
	}
}


void AStarmark_GameMode::EndOfBattle_Implementation()
{

}