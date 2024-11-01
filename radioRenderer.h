#ifndef RADIO_RENDERER
#define RADIO_RENDERER

#include "TileSheet.h"
#include "SpriteSheet.h"
#include "radioManager.h"

class Player;
class TextRenderer;

class RadioRenderer
{
	public:
	RadioRenderer(Surface* p_ptrScreen, const Player* p_player, TextRenderer* p_textRenderer);
	//~RadioRenderer();

	void Render();
	void Update(float p_deltaTime);

	private:
	SpriteSheet m_radioSpriteSheet;

	TextRenderer* m_textRenderer;
	Surface* m_ptrScreen;
	SpriteSheet m_radioNumSpriteSheet;
	SpriteSheet m_radioPortraits;
	SpriteSheet m_radioModeSpriteSheet; //  !!! change this to tilesheet
	const Player* m_player;

	RadioManager::RadioActors m_currentConversationActor = RadioManager::RadioActors::snake;
	const RadioManager::voiceLine* lastVoiceLine = nullptr;

	const int TEXT_ROW_REVEAL_SPEED = 1500;
	const float TALK_STOP_VAL = 250;

	float m_textCounter = 0;



};


#endif // !RADIO_RENDERER
