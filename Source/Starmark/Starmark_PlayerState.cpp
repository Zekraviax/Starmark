#include "Starmark_PlayerState.h"

#include "Widget_HUD_Battle.h"
#include "Character_Pathfinder.h"


// ------------------------- Battle
void AStarmark_PlayerState::CreateDefaultPlayerParty()
{
	FAvatar_Struct* DefaultAvatar = AvatarDataTable->FindRow<FAvatar_Struct>(TEXT("BalanceBoy"), "");
	PlayerState_PlayerParty.Add(*DefaultAvatar);
}


void AStarmark_PlayerState::GetPlayerControllerVariables()
{

}

