#include "Starmark_GameMode.h"

#include "Character_Pathfinder.h"


void AStarmark_GameMode::GameMode_LaunchAttack_Implementation(ACharacter_Pathfinder* Attacker, ACharacter_Pathfinder* Defender)
{
	int CurrentDamage = 1;

	CurrentDamage = FMath::CeilToInt(Attacker->AvatarData.BaseStats.Attack * 50);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 1 = %d"), CurrentDamage));
	CurrentDamage = FMath::CeilToInt(CurrentDamage / Defender->AvatarData.BaseStats.Defence);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 2 = %d"), CurrentDamage));
	CurrentDamage = FMath::CeilToInt((Attacker->AvatarData.BaseStats.Power / 2) * CurrentDamage);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 3 = %d"), CurrentDamage));
	CurrentDamage = FMath::CeilToInt(CurrentDamage / 8);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Current Damage 3 = %d"), CurrentDamage));

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Final Damage  = %d"), CurrentDamage));

	if (Defender) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Defender Valid")));
		Defender->AvatarData.CurrentHealthPoints -= CurrentDamage;
		Defender->UpdatePlayerBattleHUD();
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Defender Not Valid")));
	}
}