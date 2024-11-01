#ifndef GENERAL_UI_RENDERER
#define GENERAL_UI_RENDERER

#include "MapManager.h"
#include "SpriteSheet.h"

class EnemyManager;
class TextRenderer;
class Player;


class GeneralUIRenderer
{
	public:
	GeneralUIRenderer(Surface* p_ptrScreen, EnemyManager* p_enemyManager, TextRenderer* p_textRenderer, MapManager* p_mapManager, Player* p_player);
	//~GeneralUIRenderer();
	void Render();

	void RenderRaderTile(int2 p_pos, int2 p_size, Surface* p_mask);

	private:
	Surface* m_ptrScreen;
	EnemyManager* m_enemyManager;
	TextRenderer* m_textRenderer;
	Player* m_player;
	const float RADAR_RENDER_START_X = 1000;
	MapManager::RoomStruct* adjacentRooms[9] = { nullptr };
	MapManager* m_mapManager;
	SpriteSheet m_itemBannerSpritesheet;
	SpriteSheet m_callSprite;

};


#endif // !GENERAL_UI_RENDERER
