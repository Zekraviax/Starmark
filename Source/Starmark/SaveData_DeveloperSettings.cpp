#include "SaveData_DeveloperSettings.h"

#include "Engine/World.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"


void USaveData_DeveloperSettings::SavePlayerDataToJson()
{
	FString SaveGamesFolderPathAppend = "SaveGames/";

	// Get the project's save folder directory
	PlayerDataSaveFilePath = FPaths::ProjectSavedDir();
	UE_LOG(LogTemp, Warning, TEXT("FilePaths: ProjectSavedDir: %s"), *PlayerDataSaveFilePath);
	PlayerDataSaveFilePath.Append(SaveGamesFolderPathAppend);
	UE_LOG(LogTemp, Warning, TEXT("FilePaths: Player's Save Data Folder: %s"), *PlayerDataSaveFilePath);

	FString DevSettingsJson;
	FJsonObjectConverter::UStructToJsonObjectString(DevSettingsStruct, DevSettingsJson, 0, 0);

	// Before we save the json file, we need to check if the player's save data folder exists
	// If it doesn't, we make it first
	// The directory path should be PlayerDataSaveFilePath + the player character's name
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	bool OverwriteFile = true;

	if (!FileManager.DirectoryExists(*PlayerDataSaveFilePath)) {
		if (FileManager.CreateDirectory(*PlayerDataSaveFilePath)) {
			UE_LOG(LogTemp, Warning, TEXT("Player's save data folder did not exist but was created successfully."));
		} else {
			UE_LOG(LogTemp, Error, TEXT("Player's save data folder does not exist and could not be created."));
		}
	}

	if (FileManager.DirectoryExists(*PlayerDataSaveFilePath)) {
		FString FileName = PlayerDataSaveFilePath.Append("DeveloperSettings.json");
		UE_LOG(LogTemp, Warning, TEXT("FilePaths: Player's save data file name: %s"), *FileName);

		if (FFileHelper::SaveStringToFile(DevSettingsJson, *FileName)) {
			UE_LOG(LogTemp, Warning, TEXT("Player's data saves successfully."));
		} else {
			UE_LOG(LogTemp, Error, TEXT("Error: Failed to save Player's data."));
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("Error: Could not save Player's data."));
	}

	//BeginDestroy();
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

	//BeginDestroy();
}