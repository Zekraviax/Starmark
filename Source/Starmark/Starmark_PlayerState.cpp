#include "Starmark_PlayerState.h"


#include "Character_Pathfinder.h"
#include "PlayerController_Battle.h"
#include "PlayerController_Lobby.h"
#include "SaveData_PlayerProfile.h"
#include "Starmark_GameMode.h"
#include "Starmark_GameInstance.h"
#include "Widget_HUD_Battle.h"


void AStarmark_PlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / Tick / Net owning player: %s"), *GetNetOwningPlayer()->GetName());
}


AStarmark_PlayerState::AStarmark_PlayerState()
{
	bReplicates = true;
}


void AStarmark_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStarmark_PlayerState, PlayerReadyStatus);
	DOREPLIFETIME(AStarmark_PlayerState, PlayerTeam);
	DOREPLIFETIME(AStarmark_PlayerState, PlayerData);
	DOREPLIFETIME(AStarmark_PlayerState, ReplicatedMultiplayerUniqueID);
}


FPlayer_Data AStarmark_PlayerState::GetPlayerDataFromGameInstance()
{
	if (!GameInstanceReference) {
		GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}
	
	PlayerData.PlayerName = GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.PlayerName;
	PlayerData.ProfileName = GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.ProfileName;
	PlayerData.CurrentAvatarTeam = GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team;
	
	return PlayerData;
}


// ------------------------- Player
void AStarmark_PlayerState::UpdatePlayerData()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / UpdatePlayerData / Function called"));
	//UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / UpdatePlayerData / IsValid(GetWorld()) returns: %s"), IsValid(GetWorld()) ? TEXT("true") : TEXT("false"));
	//UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / UpdatePlayerData / IsValid(GetGameInstance) returns: %s"), IsValid(UGameplayStatics::GetGameInstance(GetWorld())) ? TEXT("true") : TEXT("false"));

	if (GetWorld()) {
		if (UGameplayStatics::GetGameInstance(GetWorld())) {
			GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

			if (Cast<APlayerController_Lobby>(GetPawn()->Controller)) {
				Cast<APlayerController_Lobby>(GetPawn()->Controller)->PlayerDataStruct = PlayerData;
			}

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / UpdatePlayerData / IsValid(PlayerSaveGameReference) returns: %s"), IsValid(GameInstanceReference->PlayerSaveGameReference) ? TEXT("true") : TEXT("false"));
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / UpdatePlayerData / Send the update to the PlayerState blueprint"));
		}
	}

	PreBattleCheck = true;
}


// ------------------------- Lobby
void AStarmark_PlayerState::SendUpdateToMultiplayerLobby_Implementation()
{
	// Implemented in Blueprints

	// What is done here?:
}


// ------------------------- Battle
// To-Do: Move this to the GameMode
void AStarmark_PlayerState::Server_SubtractHealth_Implementation(ACharacter_Pathfinder* Defender, int DamageDealt)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_SubtractHealth / IsValid(Defender) returns: %s"), IsValid(Defender) ? TEXT("true") : TEXT("false"));

	if (IsValid(Defender)) {
		Defender->AvatarData.CurrentHealthPoints -= DamageDealt;
		Defender->UpdateAvatarDataInPlayerParty();

		if (Defender->AvatarData.CurrentHealthPoints <= 0) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Avatar Defeated")));

			Battle_AvatarDefeated(Defender);
		}
	}
}


// To-Do: Move this to the GameMode
void AStarmark_PlayerState::Server_AddHealth_Implementation(ACharacter_Pathfinder* Avatar, int Healing)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_AddHealth / IsValid(Defender) returns: %s"), IsValid(Avatar) ? TEXT("true") : TEXT("false"));

	if (IsValid(Avatar)) {
		Avatar->AvatarData.CurrentHealthPoints += Healing;
		Avatar->UpdateAvatarDataInPlayerParty();

		if (Avatar->AvatarData.CurrentHealthPoints > Avatar->AvatarData.BattleStats.MaximumHealthPoints)
			Avatar->AvatarData.CurrentHealthPoints = Avatar->AvatarData.BattleStats.MaximumHealthPoints;
	}
}


// To-Do: Delete this function now that it has been moved to the GameMode.
void AStarmark_PlayerState::Battle_AvatarDefeated_Implementation(ACharacter_Pathfinder* Avatar)
{
	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Server_AvatarDefeated(Avatar);

}


// To-Do: Figure out of this function is needed, and if not, delete it.
void AStarmark_PlayerState::Server_UpdatePlayerStateVariables_Implementation(const TArray<FAvatar_Struct>& UpdatedPlayerParty)
{
	PlayerTeam = UpdatedPlayerParty;

	for (int i = 0; i < UpdatedPlayerParty.Num(); i++) {
		if (UpdatedPlayerParty[i].AvatarName != "Default") {
			UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerStateVariables / Found Avatar %s in Player %s's PlayerState"), *UpdatedPlayerParty[i].AvatarName, *GetPlayerName());
		}
	}
}
