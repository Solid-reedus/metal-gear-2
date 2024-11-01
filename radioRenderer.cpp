#include "precomp.h"
#include "radioRenderer.h"
#include "Player.h"
#include "TextRenderer.h"
#include "radioManager.h"

RadioRenderer::RadioRenderer(Surface* p_ptrScreen, const Player* p_player, TextRenderer* p_textRenderer)
{
	m_ptrScreen = p_ptrScreen;
	m_player = p_player;

	m_textRenderer = p_textRenderer;

	m_radioSpriteSheet = SpriteSheet({1,1}, "assets/radio.png", m_ptrScreen);
	m_radioNumSpriteSheet = SpriteSheet({ 5, 2 }, "assets/radioNumbers.png", m_ptrScreen);
	m_radioModeSpriteSheet = SpriteSheet({ 1, 2 }, "assets/radioMode.png", m_ptrScreen);
	m_radioPortraits = SpriteSheet({ 2, 4 }, "assets/radioPortaits2.png", m_ptrScreen);


}



int StringLength(const char* str) 
{
	int length = 0;
	while (str[length] != '\0') {
		length++;
	}
	return length;
}



void RadioRenderer::Render()
{

	m_radioSpriteSheet.renderFromScaled({ 0,0 }, { 300, 50 }, { 225, 450 });

	m_radioPortraits.renderFromScaled({ 0,0 }, { 75, 100 }, { 175 , 300 });

	const uchar index = m_player->GetRadioIndex();
	//index A and B are for knowing fow which value to render based on the spritesheet
	int2 indexA = { (index / 10) % 5, (index / 10) / 5 };
	int2 indexB = { (index % 10) % 5, (index % 10) / 5 };
	m_radioNumSpriteSheet.renderFromScaled(indexA, { 440, 272 }, { 23, 45 });
	m_radioNumSpriteSheet.renderFromScaled(indexB, { 469, 272 }, { 23, 45 });

	m_radioModeSpriteSheet.renderFromScaled({ 0,0 }, { 410, 247 }, { 85, 20 });
	
	//DARK_BLUE
	//ROOM_SIZE_IN_PIXELS
	m_ptrScreen->Bar(MAP_RENDERING_START, ROOM_SIZE_IN_PIXELS - 200, ROOM_SIZE_IN_PIXELS, ROOM_SIZE_IN_PIXELS, DARK_BLUE);
	for (int i = 0; i < 3; i++)
	{
		m_ptrScreen->Box(MAP_RENDERING_START + i, ROOM_SIZE_IN_PIXELS + i - 200, ROOM_SIZE_IN_PIXELS + i, ROOM_SIZE_IN_PIXELS + i, 0xffffff);
	}


	const RadioManager::voiceLine* voiceLine = m_player->LookAtCurrentVoiceLine();

	if (voiceLine != nullptr)
	{
		if (voiceLine->actor != RadioManager::RadioActors::snake)
		{
			m_currentConversationActor = voiceLine->actor;
		}
		if (voiceLine != lastVoiceLine)
		{
			m_textCounter = static_cast<float>(voiceLine->lines->size * TEXT_ROW_REVEAL_SPEED);
		}


		int isTalking = (static_cast<int>(m_textCounter) % 250 < 100);

		if (m_textCounter > (voiceLine->lines->size * TEXT_ROW_REVEAL_SPEED) - TALK_STOP_VAL)
		{
			isTalking = 0;
		}
		if (static_cast<int>(m_textCounter) % TEXT_ROW_REVEAL_SPEED == 0)
		{
			isTalking = 0;
		}

		switch (m_currentConversationActor)
		{
			case RadioManager::RadioActors::campel:
			{
				m_radioPortraits.renderFromScaled({ isTalking,1 }, { 575, 100 }, { 175 , 300 });
				break;
			}
			case RadioManager::RadioActors::meryl:
			{
				m_radioPortraits.renderFromScaled({ isTalking,2 }, { 575, 100 }, { 175 , 300 });
				break;
			}
			case RadioManager::RadioActors::snake:
			{
				m_radioPortraits.renderFromScaled({ isTalking,0 }, { 75, 100 }, { 175 , 300 });
				break;
			}
			default:
			break;
		}


		switch (voiceLine->actor)
		{
			case RadioManager::RadioActors::snake:
			default:
			{
				m_textRenderer->RenderText({ MAP_RENDERING_START + 15, 580 }, { 25, 25 }, "snake");
				break;
			}
			case RadioManager::RadioActors::campel:
			{
				m_radioModeSpriteSheet.renderFromScaled({ 0,1 }, { 410, 247 }, { 85, 20 });
				m_textRenderer->RenderText({ MAP_RENDERING_START + 15, 580 }, { 25, 25 }, "campb");
				break;
			}
			case RadioManager::RadioActors::meryl:
			{
				m_radioModeSpriteSheet.renderFromScaled({ 0,1 }, { 410, 247 }, { 85, 20 });
				m_textRenderer->RenderText({ MAP_RENDERING_START + 15, 580 }, { 25, 25 }, "meryl");
				break;
			}
		}

		for (int i = 0; i < voiceLine->lines->size; i++)
		{
			int maxRow = voiceLine->lines->size;
			int currentRow = maxRow - static_cast<int>(m_textCounter / TEXT_ROW_REVEAL_SPEED);

			// Ensure currentRow is within valid bounds
			if (currentRow < 1) currentRow = 1;
			if (currentRow > maxRow) currentRow = maxRow;

			if (i + 1 > currentRow)
			{
				break;
			}
			else if (i + 1 == currentRow)
			{
				// Calculate completion for the current row and clamp it to [0, 1]
				float completion = 1.0f - (static_cast<float>(static_cast<int>(m_textCounter) % TEXT_ROW_REVEAL_SPEED) / TEXT_ROW_REVEAL_SPEED);
				if (completion < 0.0f) completion = 0.0f;
				if (completion > 1.0f) completion = 1.0f;

				int textLength = StringLength(voiceLine->lines->data[i]);
				int renderedCharsCount = static_cast<int>(textLength * completion) + 1;

				// Clamp renderedCharsCount to the text length
				if (renderedCharsCount > textLength) renderedCharsCount = textLength;

				// Copy characters up to renderedCharsCount
				char renderedChars[50];
				for (int j = 0; j < renderedCharsCount; j++)
				{
					renderedChars[j] = voiceLine->lines->data[i][j];
				}
				// Null-terminate the clipped string
				renderedChars[renderedCharsCount] = '\0';

				// Render the clipped text
				m_textRenderer->RenderText({ 220, 580 + (i * 40) }, { 30, 30 }, renderedChars);
				break;
			}

			// Render fully visible rows
			m_textRenderer->RenderText({ 220, 580 + (i * 40) }, { 30, 30 }, voiceLine->lines->data[i]);
		}

		lastVoiceLine = voiceLine;


	}
	else
	{
		// easy and dirty way to reset the currentConversationActor to snake (aka no response)
		m_currentConversationActor = RadioManager::RadioActors::snake;
		m_textCounter = 0;

	}

	m_ptrScreen->Box(75, 100, 250, 400, LIGHT_GREEN);
	m_ptrScreen->Box(575, 100, 750, 400, LIGHT_GREEN);



}

void RadioRenderer::Update(float p_deltaTime)
{
	if (m_textCounter > 0)
	{
		m_textCounter -= p_deltaTime;
	}

}
