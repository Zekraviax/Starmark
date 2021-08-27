#include "Starmark_PlayerState.h"


#include "Widget_HUD_Battle.h"
#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "Player_SaveData.h"
#include "Starmark_GameState.h"


AStarmark_PlayerState::AStarmark_PlayerState()
{
	bReplicates = true;
}


void AStarmark_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStarmark_PlayerState, PlayerReadyStatus);
}


// ------------------------- Player
void AStarmark_PlayerState::UpdatePlayerData(UPlayer_SaveData* PlayerProfile)
{
	if (PlayerProfile->IsValidLowLevel()) {
		PlayerProfileReference = PlayerProfile;
	}

	if (PlayerProfileReference->IsValidLowLevel()) {
		SetPlayerName(PlayerProfileReference->Name);
	}
}


// ------------------------- Lobby
void AStarmark_PlayerState::ChangePlayerReadyStatus()
{
	if (PlayerReadyStatus == "Not Ready") {
		PlayerReadyStatus = "Ready";
	} else if (PlayerReadyStatus == "Ready") {
		PlayerReadyStatus = "Not Ready";
	} else {
		PlayerReadyStatus = "Not Ready";
	}
}


// ------------------------- Battle
void AStarmark_PlayerState::PlayerState_BeginBattle()
{
	FAvatar_Struct* DefaultAvatar = AvatarDataTable->FindRow<FAvatar_Struct>(TEXT("BalanceBoy"), "");
	PlayerState_PlayerParty.Add(*DefaultAvatar);
}


void AStarmark_PlayerState::Server_SubtractHealth_Implementation(ACharacter_Pathfinder* Defender, int DamageDealt)
{
	if (Defender) {
		Defender->AvatarData.CurrentHealthPoints -= DamageDealt;
		Defender->UpdateAvatarDataInPlayerParty();

		if (Defender->AvatarData.CurrentHealthPoints <= 0) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Avatar Defeated")));

			Battle_AvatarDefeated(Defender);
		}
	}
}


void AStarmark_PlayerState::Battle_AvatarDefeated_Implementation(ACharacter_Pathfinder* Avatar)
{
	if (Avatar->PlayerControllerReference->PlayerParty.IsValidIndex(Avatar->IndexInPlayerParty)) {
		Avatar->PlayerControllerReference->PlayerParty.RemoveAt(Avatar->IndexInPlayerParty);
	}

	if (Avatar->PlayerControllerReference->PlayerParty.Num() <= 0) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player has run out of Avatars")));
		Cast<AStarmark_GameState>(GetWorld()->GetGameState())->EndOfBattle_Implementation();
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player Avatars Remaining: %d"), Avatar->PlayerControllerReference->PlayerParty.Num()));
	}

	Avatar->Destroy();
}