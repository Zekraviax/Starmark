#include "PlayerState_Base.h"


#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "Player_SaveData.h"


void APlayerState_Base::BeginPlay()
{
	Super::BeginPlay();

	SetPlayerName("Default");
}


// ------------------------- Player
//void APlayerState_Base::UpdatePlayerData(UPlayer_SaveData* PlayerProfile)
//{
//	if (PlayerProfile->IsValidLowLevel()) {
//		PlayerProfileReference = PlayerProfile;
//	}
//
//	if (PlayerProfileReference->IsValidLowLevel()) {
//		SetPlayerName(PlayerProfileReference->Name);
//	}
//}