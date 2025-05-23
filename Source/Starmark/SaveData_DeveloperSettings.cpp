#include "SaveData_DeveloperSettings.h"

#include "Character_Pathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"


void USaveData_DeveloperSettings::SavePlayerDataToJson()
{
	FString SaveGamesFolderPathAppend = "SaveGames/";
	FString SavePath = "C:\\Users\\zekra\\Documents\\UE\\Projects\\Starmark\\Saved\\SaveGames";

	// Get the project's save folder directory
	SavePath = FPaths::ProjectSavedDir();
	UE_LOG(LogTemp, Warning, TEXT("FilePaths: ProjectSavedDir: %s"), *SavePath);
	SavePath.Append(SaveGamesFolderPathAppend);
	UE_LOG(LogTemp, Warning, TEXT("FilePaths: Player's Save Data Folder: %s"), *SavePath);

	FString DevSettingsJson;
	FJsonObjectConverter::UStructToJsonObjectString(DevSettingsStruct, DevSettingsJson, 0, 0);

	// Before we save the json file, we need to check if the player's save data folder exists.
	// If it doesn't, we make it first.
	// The directory path should be SavePath + "DeveloperSettings".
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.DirectoryExists(*SavePath)) {
		if (FileManager.CreateDirectory(*SavePath)) {
			UE_LOG(LogTemp, Warning, TEXT("Player's save data folder did not exist but was created successfully."));
		} else {
			UE_LOG(LogTemp, Error, TEXT("Player's save data folder does not exist and could not be created."));
		}
	}

	if (FileManager.DirectoryExists(*SavePath)) {
		FString FileName = SavePath.Append("DeveloperSettings.json");
		UE_LOG(LogTemp, Warning, TEXT("FilePaths: Player's save data file name: %s"), *FileName);

		if (FFileHelper::SaveStringToFile(DevSettingsJson, *FileName)) {
			UE_LOG(LogTemp, Warning, TEXT("Player's data saves successfully."));
		} else {
			UE_LOG(LogTemp, Error, TEXT("Error: Failed to save Player's data."));
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("Error: Could not save Player's data."));
	}
}


void USaveData_DeveloperSettings::LoadPlayerDataFromJson()
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString PlayerSaveDataFileName = "SaveGames/DeveloperSettings.json";

	FString PlayerDataSaveFilePath = FPaths::ProjectSavedDir();
	PlayerDataSaveFilePath.Append(PlayerSaveDataFileName);
	UE_LOG(LogTemp, Warning, TEXT("FilePaths: Player's Save Data Folder: %s"), *PlayerDataSaveFilePath);

	//FString FileName = PlayerDataSaveFilePath.Append("/Player.json");
	//UE_LOG(LogTemp, Warning, TEXT("FilePaths: Player's Save Data File: %s"), *FileName);

	if (!FileManager.FileExists(*PlayerDataSaveFilePath)) {
		UE_LOG(LogTemp, Error, TEXT("Error: Could not find Player's data."));
		return;
	}

	FString PlayerDataAsJson;
	FDeveloperSettingsAsStruct PlayerDataAsStruct;

	FFileHelper::LoadFileToString(PlayerDataAsJson, *PlayerDataSaveFilePath);
	FJsonObjectConverter::JsonObjectStringToUStruct(PlayerDataAsJson, &PlayerDataAsStruct, 0, 0);

	// Apply player data
	DevSettingsStruct = PlayerDataAsStruct;
}


void USaveData_DeveloperSettings::DumpMultiplayerStateToJsonLog_Implementation()
{
	FString SaveGamesFolderPathAppend = "SaveGames/";
	FString SavePath = "C:\\Users\\zekra\\Documents\\UE\\Projects\\Starmark\\Saved\\SaveGames";

	// Get the project's save folder directory
	SavePath = FPaths::ProjectSavedDir();
	UE_LOG(LogTemp, Warning, TEXT("FilePaths: ProjectSavedDir: %s"), *SavePath);
	SavePath.Append(SaveGamesFolderPathAppend);
	UE_LOG(LogTemp, Warning, TEXT("FilePaths: Player's Save Data Folder: %s"), *SavePath);


	

	FString DumpToLogsJson;
	FString SingleObjectJson;
	// Get all players and avatars and add their data to the FString
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), ActorList);
	for (int i = 0; i < ActorList.Num(); i++) {
		//UScriptStruct* Struct = Cast<ACharacter_Pathfinder>(ActorList[i])->AvatarData.StaticStruct();
		//FString Output = TEXT("");
		//Struct->ExportText(Output, Cast<ACharacter_Pathfinder>(ActorList[i])->AvatarData.StaticStruct(), nullptr, this, (PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient), nullptr);
		//DumpToLogsJson.Append(Output);

		FJsonObjectConverter::UStructToJsonObjectString(Cast<ACharacter_Pathfinder>(ActorList[i])->AvatarData, SingleObjectJson, 0, 0);
		DumpToLogsJson.Append(SingleObjectJson);
		UE_LOG(LogTemp, Warning, TEXT("Append Avatar data to log as FString: %s"), *SingleObjectJson);
	}
	
	

	// Before we save the json file, we need to check if the player's save data folder exists.
	// If it doesn't, we make it first.
	// The directory path should be SavePath + "DeveloperSettings".
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.DirectoryExists(*SavePath)) {
		if (FileManager.CreateDirectory(*SavePath)) {
			UE_LOG(LogTemp, Warning, TEXT("Player's save data folder did not exist but was created successfully."));
		} else {
			UE_LOG(LogTemp, Error, TEXT("Player's save data folder does not exist and could not be created."));
		}
	}

	if (FileManager.DirectoryExists(*SavePath)) {
		FString FileName = SavePath.Append("Log.json");
		UE_LOG(LogTemp, Warning, TEXT("FilePaths: Log file name: %s"), *FileName);

		if (FFileHelper::SaveStringToFile(DumpToLogsJson, *FileName)) {
			UE_LOG(LogTemp, Warning, TEXT("Log saved successfully."));
		} else {
			UE_LOG(LogTemp, Error, TEXT("Error: Failed to save Log."));
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("Error: Could not save Log."));
	}
}
