#include "Starmark_PlayerState.h"

#include "Widget_HUD_Battle.h"
#include "Character_Pathfinder.h"


// ------------------------- Battle
void AStarmark_PlayerState::CheckVariables()
{
	if (PlayerState_BattleHUD->IsValidLowLevel())
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("BattleHUD Valid")));
	else 
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("BattleHUD Not Valid")));

	if (PlayerState_CurrentControlledAvatar->IsValidLowLevel())
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("CurrentControlledAvatar Valid")));
	else
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("CurrentControlledAvatar Not Valid")));
}