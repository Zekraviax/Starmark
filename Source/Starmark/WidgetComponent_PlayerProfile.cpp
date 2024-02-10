#include "WidgetComponent_PlayerProfile.h"

#include "Kismet/GameplayStatics.h"
#include "Player_SaveData.h"
#include "PlayerController_Lobby.h"
#include "Starmark_GameInstance.h"
#include "SaveData_PlayerProfilesList.h"
#include "Starmark_PlayerState.h"
#include "Widget_PlayerProfileCreator.h"


// ------------------------- Components
void UWidgetComponent_PlayerProfile::OnSelectProfileButtonPressed()
{
	USaveData_PlayerProfilesList* PlayerProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));
	UPlayer_SaveData* FoundProfile;

	for (int i = 0; i < PlayerProfilesList->PlayerProfileNames.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("UWidgetComponent_PlayerProfile / OnSelectProfileButtonPressed / Checking profile: %s"), *PlayerProfilesList->PlayerProfileNames[i]);

		if (PlayerProfilesList->PlayerProfileNames[i] == ProfileNameText->GetText().ToString()) {
			FoundProfile = Cast<UPlayer_SaveData>(UGameplayStatics::LoadGameFromSlot(PlayerProfilesList->PlayerProfileNames[i], 0));

			UE_LOG(LogTemp, Warning, TEXT("UWidgetComponent_PlayerProfile / OnSelectProfileButtonPressed / Loaded profile: %s"), *PlayerProfilesList->PlayerProfileNames[i]);

			AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayerState());
			PlayerStateReference->PlayerProfileReference = FoundProfile;

			// Set the Player Struct data
			PlayerStateReference->PlayerDataStruct.ProfileName = PlayerProfilesList->PlayerProfileNames[i];
			PlayerStateReference->PlayerDataStruct.PlayerName = FoundProfile->Name;
			PlayerStateReference->PlayerDataStruct.CurrentAvatarTeam = FoundProfile->CurrentAvatarTeam;

			// Set the player data in the game instance as well as the player state
			Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->PlayerData = PlayerStateReference->PlayerDataStruct;

			UE_LOG(LogTemp, Warning, TEXT("UWidgetComponent_PlayerProfile / OnSelectProfileButtonPressed / Player name: %s"), *PlayerStateReference->PlayerDataStruct.PlayerName);
			UE_LOG(LogTemp, Warning, TEXT("UWidgetComponent_PlayerProfile / OnSelectProfileButtonPressed / Avatars in team: %d"), PlayerStateReference->PlayerDataStruct.CurrentAvatarTeam.Num());

			Cast<APlayerController_Lobby>(GetWorld()->GetFirstPlayerController())->PlayerDataStruct = PlayerStateReference->PlayerDataStruct;

			break;
		}
	}
	
	//return ReturnProfile;
	if (OnPlayerProfileLoadedDelegate.IsBound()) {
		OnPlayerProfileLoadedDelegate.Broadcast();
	}
}