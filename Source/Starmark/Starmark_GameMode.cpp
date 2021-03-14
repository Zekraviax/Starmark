#include "Starmark_GameMode.h"

#include "Character_Pathfinder.h"


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


void AStarmark_GameMode::Combat_SubtractHealth_Implementation(ACharacter_Pathfinder* Defender, int DamageDealt)
{
	if (Defender) {
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Defender Valid")));
		Defender->AvatarData.CurrentHealthPoints -= DamageDealt;
		//Defender->UpdatePlayerBattleHUD();

		if (Defender->AvatarData.CurrentHealthPoints <= 0) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Avatar Deleted")));
			//Defender->Destroy();
		}
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Defender Not Valid")));
	}
}