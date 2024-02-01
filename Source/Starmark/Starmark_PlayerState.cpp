#include "Starmark_PlayerState.h"


#include "Character_Pathfinder.h"
#include "PlayerController_Battle.h"
#include "Player_SaveData.h"
#include "Starmark_GameMode.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameState.h"
#include "Widget_HUD_Battle.h"


void AStarmark_PlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AStarmark_PlayerState* Player = Cast<AStarmark_PlayerState>(PlayerState);
	if (Player) {
		Player->PlayerProfileReference = PlayerProfileReference;
	}
}


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
			if (!GameInstanceReference) {
				GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			}

			ReplicatedPlayerName = GameInstanceReference->PlayerName;
			PlayerProfileReference = GameInstanceReference->CurrentProfileReference;
			PlayerState_PlayerParty = GameInstanceReference->CurrentProfileReference->CurrentAvatarTeam;

			/*
			if (!GameInstanceReference->CurrentProfileReference->IsValidLowLevel()) {
				// If the player doesn't have a profile loaded, attempt to load a default profile.
				GameInstanceReference->CurrentProfileReference = Cast<UPlayer_SaveData>(UGameplayStatics::LoadGameFromSlot("DefaultProfile", 0));
				UGameplayStatics::DeleteGameInSlot("DefaultProfile", 0);

				// Check if a default profile exists. If not, create one.
				if (!GameInstanceReference->CurrentProfileReference->IsValidLowLevel()) {
					UPlayer_SaveData* DefaultProfile = Cast<UPlayer_SaveData>(UGameplayStatics::CreateSaveGameObject(UPlayer_SaveData::StaticClass()));

					// Add all available explorers to the default profile
					if (ExplorersDataTableRowNames.Num() == 0) {
						ExplorersDataTableRowNames = ExplorersDataTable->GetRowNames();
					}

					for (const FName ExplorerRowName : ExplorersDataTableRowNames) {
						FNetscapeExplorer_Struct* ExplorerDataTableRow = ExplorersDataTable->FindRow<FNetscapeExplorer_Struct>(ExplorerRowName, PlayerStateContextString);
						FNetscapeExplorer_Struct Explorer = *ExplorerDataTableRow;

						DefaultProfile->Explorers.Add(*ExplorerDataTableRow);

						if (DefaultProfile->CurrentExplorerTeam.Num() < 4) {
							Explorer.IndexInPlayerLibrary = DefaultProfile->CurrentExplorerTeam.Num();

							// Apply formulae to stats
							// Battle Stats
							// Total Battle Stat = Base Battle Stat x (Social Stat/Number) + Level
							Explorer.BattleStats.Strength = (ExplorersDataTable->FindRow<FNetscapeExplorer_Struct>(ExplorerRowName, PlayerStateContextString)->BattleStats.Strength * (Explorer.SocialStats.Courage / 2)) + 1;
							Explorer.BattleStats.Endurance = (ExplorersDataTable->FindRow<FNetscapeExplorer_Struct>(ExplorerRowName, PlayerStateContextString)->BattleStats.Endurance * (Explorer.SocialStats.Diligence / 2)) + 1;
							Explorer.BattleStats.Agility = (ExplorersDataTable->FindRow<FNetscapeExplorer_Struct>(ExplorerRowName, PlayerStateContextString)->BattleStats.Agility * (Explorer.SocialStats.Empathy / 2)) + 1;
							Explorer.BattleStats.Magic = (ExplorersDataTable->FindRow<FNetscapeExplorer_Struct>(ExplorerRowName, PlayerStateContextString)->BattleStats.Magic * (Explorer.SocialStats.Insight / 2)) + 1;
							Explorer.BattleStats.Luck = (ExplorersDataTable->FindRow<FNetscapeExplorer_Struct>(ExplorerRowName, PlayerStateContextString)->BattleStats.Luck * (Explorer.SocialStats.Wit / 2)) + 1;

							// Health Points
							Explorer.BattleStats.MaximumHealthPoints += Explorer.BattleStats.Endurance;

							// Mana Points
							Explorer.BattleStats.MaximumManaPoints += Explorer.BattleStats.Magic;

							// Tile Moves
							Explorer.MaximumTileMoves = 2 + FMath::RoundToInt(Explorer.BattleStats.Agility / 5);

							DefaultProfile->CurrentExplorerTeam.Add(Explorer);
						}
					}

					// Set other variables
					DefaultProfile->Name = "DefaultProfile";
					DefaultProfile->ProfileName = "DefaultProfile";

					// Save the default slot
					UGameplayStatics::SaveGameToSlot(DefaultProfile, "DefaultProfile", 0);
					GameInstanceReference->CurrentProfileReference = DefaultProfile;
				}
				*/
			}

		PlayerProfileReference = GameInstanceReference->CurrentProfileReference;
		PlayerState_PlayerParty = GameInstanceReference->CurrentProfileReference->CurrentAvatarTeam;

		UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / IsValid(PlayerProfileReference) returns: %s"), IsValid(PlayerProfileReference) ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / ReplicatedPlayerName is: %s"), *ReplicatedPlayerName);
			
		/*
		const APawn* Pawn = GetPawn();
		while (Pawn == nullptr) {
			Pawn = GetPawn();
		}
		AController* Controller = Pawn->GetController();
		Cast<APlayerController_Battle>(Controller)->PlayerParty = GameInstanceReference->CurrentProfileReference->CurrentExplorerTeam;
		*/

		// Update player controller with team
		// GetNetOwningPlayer doesn't work on local clients (?)
		//UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / Pawn is valid: %s"), IsValid(GetPawn()) ? TEXT("true") : TEXT("false"));
		//UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / PlayerProfileReference is valid: %s"), IsValid(Cast<APlayerController_Battle>(GetWorld()->GetFirstLocalPlayerFromController())->PlayerProfileReference) ? TEXT("true") : TEXT("false"));
		//UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / GameInstanceReference is valid: %s"), IsValid(GameInstanceReference) ? TEXT("true") : TEXT("false"));
		//UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / Current Profile is: %s"), *GameInstanceReference->CurrentProfileReference->ProfileName);
		//UE_LOG(LogTemp, Warning, TEXT("Server_UpdatePlayerData / ReplicatedPlayerName is: %s"), *GetNetOwningPlayer()->GetPlayerController(GetWorld())->GetName());

		//Cast<APlayerController_Battle>(GetWorld()->GetFirstLocalPlayerFromController())->PlayerProfileReference = GameInstanceReference->CurrentProfileReference;

		SetPlayerName(GameInstanceReference->PlayerName);
		SendUpdateToMultiplayerLobby();
	}
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
	}
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
	Server_UpdatePlayerData();
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