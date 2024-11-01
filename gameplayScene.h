#ifndef GAMEPLAY_SCEME
#define GAMEPLAY_SCEME

#include "baseScene.h"
#include "MapManager.h"
#include "Player.h"
#include "levelAudioManager.h"
#include "renderingConst.h"
#include "EnemyManager.h"

class Game;
class RadioRenderer;
class InventoryRenderer;
class GeneralUIRenderer;
class CollisionManager;
class TextRenderer;
class SaveLoadManager;

class GamePlayScene : public BaseScene
{
	public:
	GamePlayScene();
	~GamePlayScene();

	enum class levelIndex
	{
		levelOneStart,

		levelOneFromTopL,
		levelOneFromTopR,

		levelTwoFromBottomL,
		levelTwoFromBottomR,
	};

	void Init(Tmpl8::Game* p_game, Surface* p_screen) override;
	void Init(Tmpl8::Game* p_game, Surface* p_screen, levelIndex p_levelIndex, float2 playerPos);
	void Init(Tmpl8::Game* p_game, Surface* p_screen, levelIndex p_levelIndex, float2 playerPos, Tmpl8::Game::PlayerLoadData p_loadData);
	void Init(Tmpl8::Game* p_game, Surface* p_screen, levelIndex p_levelIndex, float2 playerPos, int2 roomIndex, Tmpl8::Game::PlayerLoadData p_loadData);
	
	void Update(float p_deltaTime) override;
	void KeyUp(int p_keyUp) override;
	void KeyDown(int p_keyDown) override;

	private:

	enum class RenderState
	{
		gameplay,
		equipWeapon,
		equipItem,
		radio
	};

	Tmpl8::Game* m_game = nullptr;
	Surface* m_screen = nullptr;
	MapManager* m_mapManager = nullptr;
	Player* m_player = nullptr;
	LevelAudioManager* m_levelAudioManager = nullptr;
	EnemyManager* m_enemyManager = nullptr;
	RadioRenderer* m_radioRenderer = nullptr;
	InventoryRenderer* m_inventoryRenderer = nullptr;
	GeneralUIRenderer* m_generalUIRenderer = nullptr;
	CollisionManager* m_collisionManager = nullptr;
	TextRenderer* m_textRenderer = nullptr;
	SaveLoadManager* m_saveLoadmanager = nullptr;
	RenderState m_currentRenderState = RenderState::gameplay;
	float m_saveTextTimer = 0;


};

#endif // !GAMEPLAY_SCEME
