#include "precomp.h"
#include "winLoseScene.h"
#include "TextRenderer.h"
#include "SaveLoadManager.h"
#include "renderingConst.h"

WinLoseScence::~WinLoseScence()
{
	delete m_textRenderer;
	delete m_saveLoadManager;
}

void WinLoseScence::Init(Tmpl8::Game* p_game, Surface* p_screen)
{
	m_game = p_game;
	m_screen = p_screen;
	m_textRenderer = new TextRenderer(p_screen);

}

void WinLoseScence::Update(float p_deltaTime)
{
	(void)p_deltaTime; // ignore p_deltaTime

	m_textRenderer->RenderText({ 350, 100 }, { 80, 80 }, sceneText);

	m_textRenderer->RenderText({ 400, 500 }, { 50, 50 }, "try again");
	m_textRenderer->RenderText({ 400, 570 }, { 50, 50 }, "main menu");
	m_textRenderer->RenderText({ 400, 640 }, { 50, 50 }, "load save");
	m_screen->Bar(360, 510 + (m_menuIndex * 70), 380, 530 + (m_menuIndex * 70), 0xffffff);
}


void WinLoseScence::KeyDown(int p_keyDown)
{
	m_screen->Clear(0);

	switch (p_keyDown)
	{
		case GLFW_KEY_W:
		{
			if (m_menuIndex > 0)
			{
				m_menuIndex--;
			}
			break;
		}
		case GLFW_KEY_S:
		{
			if (MAX_INDEX > m_menuIndex)
			{
				m_menuIndex++;
			}
			break;
		}
		case GLFW_KEY_U:
		{
			switch (m_menuIndex)
			{
				case 0:
				{
					m_game->ChangeScene(Tmpl8::Game::Scenes::gameplay);
					break;
				}
				case 1:
				{
					m_game->ChangeScene(Tmpl8::Game::Scenes::menu);
					break;
				}
				case 2:
				{
					Tmpl8::Game::PlayerLoadData savedata = m_saveLoadManager->GetSaveFile();
					Tmpl8::Game::level loadedLevel = Tmpl8::Game::level::levelOne;
					if (savedata.levelIndex == 1)
					{
						loadedLevel = Tmpl8::Game::level::levelTwo;
					}
					int2 roomIndex = { savedata.worldPos.x / ROOM_SIZE, savedata.worldPos.y / ROOM_SIZE };
					float2 worldPos = { static_cast<float>((savedata.worldPos.x % ROOM_SIZE) * TILE_SIZE), static_cast<float>((savedata.worldPos.y % ROOM_SIZE) * TILE_SIZE) };
					m_game->LoadScene(loadedLevel, worldPos, roomIndex, savedata);
					break;
				}
				default:
				break;
			}
			break;
		}
		default:
		break;
	}
}

void WinLoseScence::SetScreen(bool p_won)
{
	if (p_won)
	{
		sceneText = "you won";
	}
	else
	{
		sceneText = "game over";
	}
}
