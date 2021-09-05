#include "Starmark_PlayerState.h"


#include "Widget_HUD_Battle.h"
#include "Character_Pathfinder.h"
#include "PlayerController_Base.h"
#include "Player_SaveData.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameState.h"
#include "SaveData_PlayerProfilesList.h"


AStarmark_PlayerState::AStarmark_PlayerState()
{
	bReplicates = true;
}


void AStarmark_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStarmark_PlayerState, PlayerReadyStatus);
	DOREPLIFETIME(AStarmark_PlayerState, ReplicatedPlayerName);
}


void AStarmark_PlayerState::LoadPlayerProfileInPlayerState(FString ProfileName)
{
	USaveData_PlayerProfilesList* PlayerProfilesSaveGameObject = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));
	UPlayer_SaveData* FoundPlayerProfile = nullptr;

	// Check if the selected profile exists and is valid
	for (int i = 0; i < PlayerProfilesSaveGameObject->PlayerProfileNames.Num(); i++) {
		if (PlayerProfilesSaveGameObject->PlayerProfileNames[i] == ProfileName) {
			FoundPlayerProfile = Cast<UPlayer_SaveData>(UGameplayStatics::LoadGameFromSlot(ProfileName, 0));

			if (FoundPlayerProfile->IsValidLowLevel()) {
				SetPlayerName(FoundPlayerProfile->Name);
				PlayerProfileReference = FoundPlayerProfile;

				UpdatePlayerData();
				Cast<UStarmark_GameInstance>(GetGameInstance())->PlayerProfileReference = PlayerProfileReference;
			}
		}
	}
}


// ------------------------- Player
void AStarmark_PlayerState::UpdatePlayerData_Implementation()
{
	if (PlayerProfileReference->IsValidLowLevel())
		ReplicatedPlayerName = PlayerProfileReference->Name;
}


// ------------------------- Battle
void AStarmark_PlayerState::PlayerState_BeginBattle()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(GetGameInstance());
	FAvatar_Struct* DefaultAvatar = AvatarDataTable->FindRow<FAvatar_Struct>(TEXT("BalanceBoy"), "");
	PlayerState_PlayerParty.Add(*DefaultAvatar);

	if (GameInstanceReference->PlayerProfileReference->IsValidLowLevel()) {
		ReplicatedPlayerName = GameInstanceReference->PlayerProfileReference->Name;
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ReplicatedPlayerName: %s"), *ReplicatedPlayerName));
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