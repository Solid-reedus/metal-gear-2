#ifndef INVENTORY
#define INVENTORY

#include "commonTypes.h"

class Player;
class SpriteSheet;
class TextRenderer;
class BasePickupable;

class InventoryRenderer
{
	public:
	InventoryRenderer(const Player* p_ptrplayer, TextRenderer* p_textRenderer, Surface* p_ptrScreen);
	void SetData(bool m_renderWeapons);
	~InventoryRenderer();

	void Render();


	private:
	const Player* m_ptrplayer;
	TextRenderer* m_textRenderer;
	Surface* m_ptrScreen;
	DymArr<const BasePickupable*>* renderedItems = nullptr;
	char* m_inventoryHeader;


	struct renderingConst
	{
		static constexpr int SELECTION_DOT_START_X = 100;
		static constexpr int SELECTION_ICON_START_X = 130;
		static constexpr int SELECTION_START_Y = 200;
		static constexpr int SELECTION_MULTI_X = 250;
		static constexpr int SELECTION_MULTI_y = 125;

		static constexpr int INVENTORY_PITCH = 3;

		static constexpr int TEXT_SIZE = 40;

	};


};


#endif // !INVENTORY
