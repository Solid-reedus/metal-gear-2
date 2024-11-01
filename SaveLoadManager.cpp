#include "precomp.h"
#include "SaveLoadManager.h"
#include "renderingConst.h"
#include <cstdio>  

// this is for making dir's
#include <sys/stat.h> 


const char* SAVE_FOLDER = "saves";
const char* SAVE_FILE_PATH = "saves/savefile.dat";


bool SaveLoadManager::directoryExists(const char* folderPath)
{
    struct stat info;
    return (stat(folderPath, &info) == 0 && (info.st_mode & S_IFDIR));
}

void SaveLoadManager::createDirectoryIfNotExists(const char* folderPath)
{
    if (!directoryExists(folderPath))
    {
        _mkdir(folderPath);
    }
}


Tmpl8::Game::PlayerLoadData SaveLoadManager::GetSaveFile()
{
    createDirectoryIfNotExists(SAVE_FOLDER);

    // Open the file in binary read mode
    FILE* file = fopen(SAVE_FILE_PATH, "rb");
    Tmpl8::Game::PlayerLoadData playerData;

    if (file != nullptr) {
        // Read the player's data from the file
        fread(&playerData, sizeof(Tmpl8::Game::PlayerLoadData), 1, file);
        fclose(file);
    }
    else 
    {
        /*
        	struct PlayerLoadData
	{
		int2 worldPos;
		int levelIndex;
		int health;
		int bullets;
		bool hasGun;
		bool isCrouching;
	};
        
        */

        // File doesn't exist, initialize with default values
        playerData = {{ROOM_SIZE_IN_PIXELS / 2, ROOM_SIZE_IN_PIXELS - (ROOM_SIZE_IN_PIXELS / 3)}, 0, 600, 0, false, false };
    }

    return playerData;
}


void SaveLoadManager::SaveGameState(Tmpl8::Game::PlayerLoadData p_saveData)
{
    createDirectoryIfNotExists(SAVE_FOLDER);

    // Open the file in binary write mode, creating or overwriting the file
    FILE* file = fopen(SAVE_FILE_PATH, "wb");
    if (file != nullptr) {
        // Write the player's data to the file
        fwrite(&p_saveData, sizeof(Tmpl8::Game::PlayerLoadData), 1, file);
        fclose(file);
    }
}
