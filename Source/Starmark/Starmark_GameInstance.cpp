#include "Starmark_GameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "SaveData_PlayerProfilesList.h"


// ------------------------- Player
void UStarmark_GameInstance::LoadProfile(FString ProfileName)
{
	USaveData_PlayerProfilesList* SaveGameObject = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

	for (int i = 0; i < SaveGameObject->PlayerProfileNames.Num(); i++) {
		if (SaveGameObject->PlayerProfileNames[i] == ProfileName) {
			PlayerName = ProfileName;
			CurrentProfileName = ProfileName;

			CurrentProfileReference = Cast<UPlayer_SaveData>(UGameplayStatics::LoadGameFromSlot(ProfileName, 0));
		}
	}
}