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
	DOREPLIFETIME(AStarmark_PlayerState, PlayerProfileReference);
}


// ------------------------- Player
void AStarmark_PlayerState::UpdatePlayerData()
{
	UE_LOG(LogTemp, Warning, TEXT("UpdatePlayerData / IsValid(GetWorld()) returns: %s"), IsValid(GetWorld()) ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("UpdatePlayerData / IsValid(GetGameInstance) returns: %s"), IsValid(UGameplayStatics::GetGameInstance(GetWorld())) ? TEXT("true") : TEXT("false"));

	//if (!GameInstanceReference)
	if (GetWorld()) {
		if (UGameplayStatics::GetGameInstance(GetWorld())) {
			GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

			ReplicatedPlayerName = GameInstanceReference->PlayerName;
			PlayerProfileReference = GameInstanceReference->CurrentProfileReference;

			UE_LOG(LogTemp, Warning, TEXT("UpdatePlayerData / IsValid(PlayerProfileReference) returns: %s"), IsValid(PlayerProfileReference) ? TEXT("true") : TEXT("false"));

			SetPlayerName(GameInstanceReference->PlayerName);
			SendUpdateToMultiplayerLobby();
		}
	}
}


void AStarmark_PlayerState::Server_UpdatePlayerData_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / IsValid(GetWorld()) returns: %s"), IsValid(GetWorld()) ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / IsValid(GetGameInstance) returns: %s"), IsValid(UGameplayStatics::GetGameInstance(GetWorld())) ? TEXT("true") : TEXT("false"));

	//if (!GameInstanceReference)
	if (GetWorld()) {
		if (UGameplayStatics::GetGameInstance(GetWorld())) {
			if (!GameInstanceReference)
				GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

			ReplicatedPlayerName = GameInstanceReference->PlayerName;
			PlayerProfileReference = GameInstanceReference->CurrentProfileReference;
			PlayerState_PlayerParty = GameInstanceReference->CurrentProfileReference->CurrentAvatarTeam;

			UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / IsValid(PlayerProfileReference) returns: %s"), IsValid(PlayerProfileReference) ? TEXT("true") : TEXT("false"));
			UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / ReplicatedPlayerName is: %s"), *ReplicatedPlayerName);

			for (int i = PlayerState_PlayerParty.Num() - 1; i >= 0; i--) {
				if (PlayerState_PlayerParty[i].AvatarName != "Default") {
					UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / PlayerState_PlayerParty member number %d is: %s"), i, *PlayerState_PlayerParty[i].AvatarName);
				} else {
					PlayerState_PlayerParty.RemoveAt(i);
					UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / Remove invalid member %d from PlayerState_PlayerParty"), i);
				}
			}

			// Update player controller
			Cast<APlayerController_Base>(GetPawn()->GetController())->PlayerParty = GameInstanceReference->CurrentProfileReference->CurrentAvatarTeam;
			Cast<APlayerController_Base>(GetPawn()->GetController())->PlayerProfileReference = GameInstanceReference->CurrentProfileReference;

			SetPlayerName(GameInstanceReference->PlayerName);
			SendUpdateToMultiplayerLobby();
		}
	}
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
	Server_UpdatePlayerStateVariables(PlayerState_PlayerParty);
}


void AStarmark_PlayerState::Server_UpdateReplicatedPlayerName_Implementation(const FString& UpdatedReplicatedPlayerName)
{
	ReplicatedPlayerName = UpdatedReplicatedPlayerName;
	SetPlayerName(UpdatedReplicatedPlayerName);
}


void AStarmark_PlayerState::PlayerState_BeginBattle_Implementation()
{
	// Retrieve player profile
	Server_UpdatePlayerData();

	//Client_UpdateReplicatedPlayerName();
}


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

	for (int i = 0; i < UpdatedPlayerParty.Num(); i++) {
		if (UpdatedPlayerParty[i].AvatarName != "Default") {
			UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerStateVariables / Found Avatar %s in Player %s's PlayerState"), *UpdatedPlayerParty[i].AvatarName, *PlayerName);
		}
	}
}