#include "precomp.h"
#include "mainMenuScene.h"
#include "TextRenderer.h"
#include "SaveLoadManager.h"
#include "renderingConst.h"

MainMenuScene::MainMenuScene()
{

}

MainMenuScene::~MainMenuScene()
{
	delete m_textRenderer;
	delete m_saveLoadManager;
}

void MainMenuScene::Init(Tmpl8::Game* p_game, Surface* p_screen)
{
	m_game = p_game;
	m_screen = p_screen;
	m_textRenderer = new TextRenderer(p_screen);
	m_saveLoadManager = new SaveLoadManager();
}

void MainMenuScene::Update(float p_deltaTime) 
{
	(void)p_deltaTime; // ignore p_deltaTime

	m_textRenderer->RenderText({ 100, 100 }, { 100, 100 }, "metal gear 2");
	m_textRenderer->RenderText({ 350, 210 }, { 30, 30 }, "made by andrzej betiuk");

	m_textRenderer->RenderText({ 400, 500 }, { 50, 50 }, "play");
	m_textRenderer->RenderText({ 400, 570 }, { 50, 50 }, "load save");
	m_screen->Bar(360, 510 + (m_menuIndex * 70), 380, 530 + (m_menuIndex * 70), 0xffffff);

	m_textRenderer->RenderText({ 50, SCRHEIGHT - 130 }, { 20, 20 }, "controls ");
	m_textRenderer->RenderText({ 50, SCRHEIGHT - 105 }, { 20, 20 }, "wasd: movement");
	m_textRenderer->RenderText({ 50, SCRHEIGHT - 80 }, { 20, 20 }, "c: crouching");
	m_textRenderer->RenderText({ 50, SCRHEIGHT - 55 }, { 20, 20 }, "u: interact/punch ");
	m_textRenderer->RenderText({ 50, SCRHEIGHT - 30 }, { 20, 20 }, "i: use weapon ");

	m_textRenderer->RenderText({ 450, SCRHEIGHT - 105 }, { 20, 20 }, "num 1: weapon inventory");
	m_textRenderer->RenderText({ 450, SCRHEIGHT - 80 }, { 20, 20 }, "num 2: item inventory");
	m_textRenderer->RenderText({ 450, SCRHEIGHT - 55 }, { 20, 20 }, "num 3: radio");
	m_textRenderer->RenderText({ 450, SCRHEIGHT -  30 }, { 20, 20 }, "num 4 save game state ");

}

void MainMenuScene::KeyDown(int p_keyDown)
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
			if (m_menuIndex == 0)
			{
				m_game->ChangeScene(Tmpl8::Game::Scenes::gameplay);
			}
			else if (m_menuIndex == 1)
			{
				Tmpl8::Game::PlayerLoadData savedata = m_saveLoadManager->GetSaveFile();
				Tmpl8::Game::level loadedLevel = Tmpl8::Game::level::levelOne;
				if (savedata.levelIndex == 1)
				{
					loadedLevel = Tmpl8::Game::level::levelTwo;
				}

				int2 roomIndex = { savedata.worldPos.x / ROOM_SIZE, savedata.worldPos.y / ROOM_SIZE };
				float2 worldPos = { static_cast<float>((savedata.worldPos.x % ROOM_SIZE) * TILE_SIZE), static_cast<float>((savedata.worldPos.y % ROOM_SIZE) * TILE_SIZE)};
				m_game->LoadScene(loadedLevel, worldPos, roomIndex, savedata);

			}

			break;
		}
		default:
		break;
	}

}
