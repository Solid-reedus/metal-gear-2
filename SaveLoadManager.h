#ifndef SAVE_LOAD_MANAGER

#include "game.h"

class SaveLoadManager
{
	public:
	SaveLoadManager() {};
	//~SaveLoadManager();

	bool directoryExists(const char* folderPath);
	void createDirectoryIfNotExists(const char* folderPath);

	Tmpl8::Game::PlayerLoadData GetSaveFile();
	void SaveGameState(Tmpl8::Game::PlayerLoadData p_saveData);

	private:

};


#endif // !SAVE_LOAD_MANAGER
