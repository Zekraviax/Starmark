#include "Starmark_PlayerState.h"


#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "Player_SaveData.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameState.h"
#include "Widget_HUD_Battle.h"


AStarmark_PlayerState::AStarmark_PlayerState()
{
	bReplicates = true;
}


void AStarmark_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStarmark_PlayerState, PlayerReadyStatus);
	DOREPLIFETIME(AStarmark_PlayerState, ReplicatedPlayerName);
	DOREPLIFETIME(AStarmark_PlayerState, PlayerState_PlayerParty);
}


// ------------------------- Player
void AStarmark_PlayerState::UpdatePlayerData()
{
	if (!GameInstanceReference)
		GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ReplicatedPlayerName = GameInstanceReference->PlayerName;
	PlayerProfileReference = GameInstanceReference->CurrentProfileReference;

	SetPlayerName(GameInstanceReference->PlayerName);
	SendUpdateToMultiplayerLobby();
}


void AStarmark_PlayerState::SaveToCurrentProfile()
{
	if (!GameInstanceReference)
		GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (!PlayerProfileReference)
		PlayerProfileReference = GameInstanceReference->CurrentProfileReference;

	if (PlayerProfileReference) {
		PlayerProfileReference->Name = GameInstanceReference->PlayerName;

		UGameplayStatics::SaveGameToSlot(PlayerProfileReference, GameInstanceReference->CurrentProfileName, 0);
	}
}


// ------------------------- Lobby
void AStarmark_PlayerState::SendUpdateToMultiplayerLobby_Implementation()
{
	// Implemented in Blueprints
}


// ------------------------- Battle
void AStarmark_PlayerState::Client_UpdateReplicatedPlayerName_Implementation()
{
	Server_UpdateReplicatedPlayerName(ReplicatedPlayerName);
}


void AStarmark_PlayerState::Server_UpdateReplicatedPlayerName_Implementation(const FString& UpdatedReplicatedPlayerName)
{
	ReplicatedPlayerName = UpdatedReplicatedPlayerName;
	SetPlayerName(UpdatedReplicatedPlayerName);
}


void AStarmark_PlayerState::PlayerState_BeginBattle_Implementation()
{
	// Retrieve player profile
	UpdatePlayerData();
	Client_UpdateReplicatedPlayerName();
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
	else 
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player Avatars Remaining: %d"), Avatar->PlayerControllerReference->PlayerParty.Num()));

	Avatar->Destroy();
}


void AStarmark_PlayerState::Server_UpdatePlayerStateVariables_Implementation(const TArray<FAvatar_Struct>& UpdatedPlayerParty)
{
	PlayerState_PlayerParty = UpdatedPlayerParty;
}