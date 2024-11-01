// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#include "precomp.h"
#include "game.h"
#include "allScenes.h"

void Tmpl8::Game::Init()
{
	screen->Clear(0);
	currentScene = new MainMenuScene();
	currentScene->Init((Tmpl8::Game*)this, screen);
}

void Tmpl8::Game::Tick(float deltaTime)
{	
	currentScene->Update(deltaTime);
}

void Tmpl8::Game::Shutdown()
{
	delete currentScene;
}

void Tmpl8::Game::KeyUp(int p_keyUp)
{
	currentScene->KeyUp(p_keyUp);
}

void Tmpl8::Game::KeyDown(int p_keyDown)
{
	currentScene->KeyDown(p_keyDown);
}

void Tmpl8::Game::ChangeScene(Scenes p_newScene)
{
	if (currentScene != nullptr)
	{
		delete currentScene;
	}

	screen->Clear(0);


	switch (p_newScene)
	{
		case Tmpl8::Game::Scenes::menu:
		{
			currentScene = new MainMenuScene();
			currentScene->Init((Tmpl8::Game*)this, screen);
			break;
		}
		case Tmpl8::Game::Scenes::gameplay:
		{
			GamePlayScene* gameplayScene = new GamePlayScene();
			int2 pos = {ROOM_SIZE_IN_PIXELS / 2, ROOM_SIZE_IN_PIXELS - (ROOM_SIZE_IN_PIXELS / 3)};
			gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelOneStart, pos);
			currentScene = dynamic_cast<BaseScene*>(gameplayScene);
			break;
		}
		case Tmpl8::Game::Scenes::winScreen:
		{
			currentScene = new WinLoseScence();
			currentScene->Init((Tmpl8::Game*)this, screen);
			WinLoseScence* scene = dynamic_cast<WinLoseScence*>(currentScene);
			scene->SetScreen(true);
			break;
		}
		case Tmpl8::Game::Scenes::loseScreen:
		{
			currentScene = new WinLoseScence();
			currentScene->Init((Tmpl8::Game*)this, screen);
			WinLoseScence* scene = dynamic_cast<WinLoseScence*>(currentScene);
			scene->SetScreen(false);
			break;
		}
		default:
		break;
	}
}

void Tmpl8::Game::ChangeScene(level p_level, int2 p_roomIndex, PlayerLoadData p_loadData)
{

	if (currentScene != nullptr)
	{
		delete currentScene;
	}

	screen->Clear(0);

	switch (p_level)
	{
		case Tmpl8::Game::level::levelOne:
		{
			if (ROOM_SIZE_IN_PIXELS > p_roomIndex.x * ROOM_SIZE_IN_PIXELS)
			{
				float2 pos = { static_cast<float>(ROOM_SIZE_IN_PIXELS / 2 - (TILE_SIZE * 2)),  static_cast<float>(TILE_SIZE * 5)};
				GamePlayScene* gameplayScene = new GamePlayScene();
				gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelOneFromTopL, pos, p_loadData);
				currentScene = dynamic_cast<BaseScene*>(gameplayScene);
			}
			else
			{
				float2 pos = { static_cast<float>(ROOM_SIZE_IN_PIXELS / 3),  static_cast<float>(TILE_SIZE * 5)};
				GamePlayScene* gameplayScene = new GamePlayScene();
				gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelOneFromTopR, pos, p_loadData);
				currentScene = dynamic_cast<BaseScene*>(gameplayScene);
			}



			break;
		}
		case Tmpl8::Game::level::levelTwo:
		{

			if (ROOM_SIZE_IN_PIXELS > p_roomIndex.x * ROOM_SIZE_IN_PIXELS)
			{
				float2 pos = { static_cast<float>(ROOM_SIZE_IN_PIXELS / 3 + TILE_SIZE),  static_cast<float>(ROOM_SIZE_IN_PIXELS - TILE_SIZE * 6)};
				GamePlayScene* gameplayScene = new GamePlayScene();
				gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelTwoFromBottomL, pos, p_loadData);
				currentScene = dynamic_cast<BaseScene*>(gameplayScene);
			}
			else
			{
				float2 pos = { static_cast<float>(ROOM_SIZE_IN_PIXELS / 3 - TILE_SIZE),  static_cast<float>(ROOM_SIZE_IN_PIXELS - TILE_SIZE * 6)};
				GamePlayScene* gameplayScene = new GamePlayScene();
				gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelTwoFromBottomR, pos, p_loadData);
				currentScene = dynamic_cast<BaseScene*>(gameplayScene);
			}
			break;
		}
	}
}

void Tmpl8::Game::ChangeScene(level p_level, float2 p_worldPos, int2 p_roomIndex, PlayerLoadData p_loadData)
{

	if (currentScene != nullptr)
	{
		delete currentScene;
	}

	screen->Clear(0);

	switch (p_level)
	{
	case Tmpl8::Game::level::levelOne:
	{


		if (ROOM_SIZE_IN_PIXELS > p_roomIndex.x * ROOM_SIZE_IN_PIXELS)
		{
			GamePlayScene* gameplayScene = new GamePlayScene();
			gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelOneFromTopL, p_worldPos, p_loadData);
			currentScene = dynamic_cast<BaseScene*>(gameplayScene);
		}
		else
		{
			GamePlayScene* gameplayScene = new GamePlayScene();
			gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelOneFromTopR, p_worldPos, p_loadData);
			currentScene = dynamic_cast<BaseScene*>(gameplayScene);
		}

		break;
	}
	case Tmpl8::Game::level::levelTwo:
	{

		if (ROOM_SIZE_IN_PIXELS > p_roomIndex.x * ROOM_SIZE_IN_PIXELS)
		{
			GamePlayScene* gameplayScene = new GamePlayScene();
			gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelTwoFromBottomL, p_worldPos, p_loadData);
			currentScene = dynamic_cast<BaseScene*>(gameplayScene);
		}
		else
		{
			GamePlayScene* gameplayScene = new GamePlayScene();
			gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelTwoFromBottomR, p_worldPos, p_loadData);
			currentScene = dynamic_cast<BaseScene*>(gameplayScene);
		}
		break;
	}
	}
}

void Tmpl8::Game::LoadScene(level p_level, float2 p_worldPos, int2 p_roomIndex, PlayerLoadData p_loadData)
{

	if (currentScene != nullptr)
	{
		delete currentScene;
	}

	screen->Clear(0);

	switch (p_level)
	{
		case Tmpl8::Game::level::levelOne:
		{

			if (ROOM_SIZE_IN_PIXELS > p_roomIndex.x * ROOM_SIZE_IN_PIXELS)
			{
				GamePlayScene* gameplayScene = new GamePlayScene();
				gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelOneFromTopL, p_worldPos, p_roomIndex, p_loadData);
				currentScene = dynamic_cast<BaseScene*>(gameplayScene);
			}
			else
			{
				GamePlayScene* gameplayScene = new GamePlayScene();
				gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelOneFromTopR, p_worldPos, p_roomIndex, p_loadData);
				currentScene = dynamic_cast<BaseScene*>(gameplayScene);
			}

			break;
		}
		case Tmpl8::Game::level::levelTwo:
		{

			if (ROOM_SIZE_IN_PIXELS > p_roomIndex.x * ROOM_SIZE_IN_PIXELS)
			{
				GamePlayScene* gameplayScene = new GamePlayScene();
				gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelTwoFromBottomL, p_worldPos, p_roomIndex, p_loadData);
				currentScene = dynamic_cast<BaseScene*>(gameplayScene);
			}
			else
			{
				GamePlayScene* gameplayScene = new GamePlayScene();
				gameplayScene->Init((Tmpl8::Game*)this, screen, GamePlayScene::levelIndex::levelTwoFromBottomR, p_worldPos, p_roomIndex, p_loadData);
				currentScene = dynamic_cast<BaseScene*>(gameplayScene);
			}
			break;
		}
	}
}

