// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#pragma once

class BaseScene;

namespace Tmpl8
{

class Game : public TheApp
{

	public:

	// game flow methods
	void Init();
	void Tick( float deltaTime );
	void Shutdown();
	// input handling
	void MouseUp( int ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y ) { mousePos.x = x, mousePos.y = y; }
	void MouseWheel( float ) { /* implement if you want to handle the mouse wheel */ }
	void KeyUp(int);
	void KeyDown(int);

	enum class Scenes
	{
		menu,
		gameplay,
		winScreen,
		loseScreen,
	};

	enum class level
	{
		levelOne,
		levelTwo,
	};

	struct PlayerLoadData
	{
		int2 worldPos;
		int levelIndex;
		int health;
		int bullets;
		bool hasGun;
		bool isCrouching;
	};

	void ChangeScene(Scenes p_newScene);
	void ChangeScene(level p_level, int2 p_roomIndex, PlayerLoadData p_loadData);
	void ChangeScene(level p_level, float2 p_worldPos, int2 p_roomIndex, PlayerLoadData p_loadData);
	void LoadScene(level p_level, float2 p_worldPos, int2 p_roomIndex, PlayerLoadData p_loadData);

	// data members
	int2 mousePos;

	private:

	BaseScene* currentScene = nullptr;

};

} // namespace Tmpl8