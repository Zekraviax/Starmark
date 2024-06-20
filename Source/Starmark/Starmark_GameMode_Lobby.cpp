#include "Starmark_GameMode_Lobby.h"

#include "PlayerController_Lobby.h"


// To-Do: Figure out why this isn't working
void AStarmark_GameMode_Lobby::HandleSeamlessTravelPlayer(AController*& C)
{
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode_Lobby / HandleSeamlessTravelPlayer / Player has finished loading"));
	UE_LOG(LogTemp, Warning, TEXT("AStarmark_GameMode_Lobby / HandleSeamlessTravelPlayer / PlayerController has data with ProfileName: %s"), *Cast<APlayerController_Lobby>(C)->PlayerDataStruct.ProfileName);
}