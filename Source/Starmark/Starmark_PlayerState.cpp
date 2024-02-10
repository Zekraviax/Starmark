#include "Starmark_PlayerState.h"


#include "Character_Pathfinder.h"
#include "PlayerController_Battle.h"
#include "PlayerController_Lobby.h"
#include "Player_SaveData.h"
#include "Starmark_GameMode.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameState.h"
#include "Widget_HUD_Battle.h"


/*
void AStarmark_PlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AStarmark_PlayerState* Player = Cast<AStarmark_PlayerState>(PlayerState);

	if (Player) {
		Player->PlayerProfileReference = PlayerProfileReference;

		if (PlayerProfileReference) {
			UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / CopyProperties / Kept the player %s ProfileReference travelling between levels (?)"), *PlayerProfileReference->ProfileName);
		}
	}
}
*/


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
	DOREPLIFETIME(AStarmark_PlayerState, PlayerDataStruct);
}


// ------------------------- Player
void AStarmark_PlayerState::UpdatePlayerData()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / UpdatePlayerData / Function called"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / UpdatePlayerData / IsValid(GetWorld()) returns: %s"), IsValid(GetWorld()) ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / UpdatePlayerData / IsValid(GetGameInstance) returns: %s"), IsValid(UGameplayStatics::GetGameInstance(GetWorld())) ? TEXT("true") : TEXT("false"));

	if (GetWorld()) {
		if (UGameplayStatics::GetGameInstance(GetWorld())) {
			
			GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			PlayerProfileReference = GameInstanceReference->CurrentProfileReference;

			// Player Data Struct
			ReplicatedPlayerName = PlayerProfileReference->Name;
			PlayerDataStruct.PlayerName = ReplicatedPlayerName;
			PlayerDataStruct.ProfileName = PlayerProfileReference->ProfileName;
			PlayerDataStruct.CurrentAvatarTeam = PlayerProfileReference->CurrentAvatarTeam;

			if (Cast<APlayerController_Lobby>(GetPawn()->Controller)) {
				Cast<APlayerController_Lobby>(GetPawn()->Controller)->PlayerDataStruct = PlayerDataStruct;
			}

			UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / UpdatePlayerData / IsValid(PlayerProfileReference) returns: %s"), IsValid(PlayerProfileReference) ? TEXT("true") : TEXT("false"));

			SetPlayerName(GameInstanceReference->PlayerName);
			SendUpdateToMultiplayerLobby();
		}
	}
}


void AStarmark_PlayerState::Server_UpdatePlayerData_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / Server_UpdatePlayerData / Function called"));
}



void AStarmark_PlayerState::Client_UpdatePlayerData_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / Client_UpdatePlayerData / Retrieve the player's data in the blueprints"));

	// To-Do: Figure out if the ProfileReference is valid after this point
	SendUpdateToMultiplayerLobby();

	GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	PlayerDataStruct = GameInstanceReference->PlayerData;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / Client_UpdatePlayerData / Finished retrieving this players' data"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / Client_UpdatePlayerData / ReplicatedPlayerName is: %s"), *ReplicatedPlayerName);
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / Client_UpdatePlayerData / Player has %d avatars"), PlayerDataStruct.CurrentAvatarTeam.Num());
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / Client_UpdatePlayerData / Applied the player's party data?"));
}


void AStarmark_PlayerState::SaveToCurrentProfile()
{
	if (!GameInstanceReference) {
		GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}

	if (!PlayerProfileReference) {
		PlayerProfileReference = GameInstanceReference->CurrentProfileReference;
	}

	if (PlayerProfileReference) {
		PlayerProfileReference->Name = GameInstanceReference->PlayerName;

		UGameplayStatics::SaveGameToSlot(PlayerProfileReference, GameInstanceReference->CurrentProfileName, 0);

		UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / SaveToCurrentProfile / Saved the players' profile"));
	}
}


void AStarmark_PlayerState::OnRepNotify_PlayerProfileReferenceUpdated()
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / OnRepNotify_PlayerProfileReferenceUpdated / PlayerProfileReference changed"));
}


UPlayer_SaveData* AStarmark_PlayerState::ReturnPlayerData()
{
	// Set the profile data in the players' controller
	//Cast<APlayerController_Battle>(GetPawn()->Controller)->PlayerParty = PlayerProfileReference->CurrentAvatarTeam;
	//Cast<APlayerController_Battle>(GetPawn()->Controller)->PlayerName = PlayerProfileReference->ProfileName;

	UE_LOG(LogTemp, Warning, TEXT("AStarmark_PlayerState / ReturnPlayerData / Players' data returned"));
	
	return PlayerProfileReference;
}


// ------------------------- Lobby
void AStarmark_PlayerState::SendUpdateToMultiplayerLobby_Implementation()
{
	// Implemented in Blueprints

	// What is done here?:
	// Retrieve the profile data locally and send it to the player's new controller
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
	Client_UpdatePlayerData();
}


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


// To-Do: Move this to the GameMode
void AStarmark_PlayerState::Battle_AvatarDefeated_Implementation(ACharacter_Pathfinder* Avatar)
{
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());
	APlayerController_Battle* PlayerControllerReference = Avatar->PlayerControllerReference;

	if (IsValid(Avatar->PlayerControllerReference)) {
		if (Avatar->PlayerControllerReference->PlayerParty.IsValidIndex(Avatar->IndexInPlayerParty)) {
			Avatar->PlayerControllerReference->PlayerParty.RemoveAt(Avatar->IndexInPlayerParty);

			// Remove the Avatar from the turn order
			UE_LOG(LogTemp, Warning, TEXT("Battle_AvatarDefeated / Remove Avatar %s from Turn Order"), *Avatar->AvatarData.AvatarName);
			GameStateReference->AvatarTurnOrder.Remove(Avatar);
			GameStateReference->DynamicAvatarTurnOrder.Remove(Avatar);

			Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Server_AssembleTurnOrderText();
		}

		if (Avatar->PlayerControllerReference->PlayerParty.Num() <= 0) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player has run out of Avatars")));
			GameStateReference->EndOfBattle_Implementation();
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player Avatars Remaining: %d"), Avatar->PlayerControllerReference->PlayerParty.Num()));

			// Check for any avatars in reserve
			bool FoundAvatarInReserve = false;
			
			for (int i = 0; i < Avatar->PlayerControllerReference->PlayerParty.Num(); i++) {
				FAvatar_Struct FoundAvatar = Avatar->PlayerControllerReference->PlayerParty[i];
				if (FoundAvatar.CurrentHealthPoints > 0 && FoundAvatar.IndexInPlayerLibrary >= 4) {
					// To-Do: Allow the player to summon an Avatar from reserve as a special action.
					FoundAvatarInReserve = true;

					PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.AttackEffectsOnTarget.Empty();
					PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.AttackEffectsOnTarget.Add(EBattle_AttackEffects::SummonAvatar);
					PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.AttackTargetsInRange = EBattle_AttackTargetsInRange::SelectAllGridTiles;
					PlayerControllerReference->CurrentSelectedAvatar->CurrentSelectedAttack.AttackPattern = EBattle_AttackPatterns::SingleTile;
					
					break;
				}
			}
		}
	}

	Avatar->Destroy();
}


void AStarmark_PlayerState::Server_UpdatePlayerStateVariables_Implementation(const TArray<FAvatar_Struct>& UpdatedPlayerParty)
{
	PlayerState_PlayerParty = UpdatedPlayerParty;

	for (int i = 0; i < UpdatedPlayerParty.Num(); i++) {
		if (UpdatedPlayerParty[i].AvatarName != "Default") {
			UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerStateVariables / Found Avatar %s in Player %s's PlayerState"), *UpdatedPlayerParty[i].AvatarName, *GetPlayerName());
		}
	}
}
