#include "Starmark_GameMode.h"

#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "Starmark_GameState.h"


// ------------------------- Battle
void AStarmark_GameMode::GameMode_LaunchAttack_Implementation(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	int CurrentDamage = 1;

	CurrentDamage = FMath::CeilToInt(Attacker->AvatarData.BaseStats.Attack * Attacker->CurrentSelectedAttack.BasePower);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 1 = %d"), CurrentDamage));
	CurrentDamage = FMath::CeilToInt(CurrentDamage / Defender->AvatarData.BaseStats.Defence);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 2 = %d"), CurrentDamage));
	CurrentDamage = FMath::CeilToInt((Attacker->AvatarData.BaseStats.Power / 2) * CurrentDamage);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 3 = %d"), CurrentDamage));
	CurrentDamage = FMath::CeilToInt(CurrentDamage / 8);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 4 = %d"), CurrentDamage));


	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Final Damage  = %d"), CurrentDamage));
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


void AStarmark_GameMode::EndOfTurn()
{
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	GameStateReference->CurrentAvatarTurnIndex++;

	if (GameStateReference->CurrentAvatarTurnIndex >= GameStateReference->AvatarTurnOrder.Num()) {
		GameStateReference->CurrentAvatarTurnIndex = 0;
	}

	for (int i = 0; i < GameStateReference->PlayerArray.Num(); i++) {

	}
}


void AStarmark_GameMode::EndOfBattle_Implementation()
{

}