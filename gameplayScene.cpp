#include "precomp.h"
#include "gameplayScene.h"

#include "Player.h"
#include "MapManager.h" 
#include "radioRenderer.h"
#include "inventoryRenderer.h"
#include "generalUIRenderer.h"
#include "collisionManager.h"
#include "TextRenderer.h"

#include "SaveLoadManager.h"
#include "game.h"

GamePlayScene::GamePlayScene()
{

}

GamePlayScene::~GamePlayScene()
{
	delete m_enemyManager;
	delete m_textRenderer;
	delete m_collisionManager;
	delete m_generalUIRenderer;
	delete m_inventoryRenderer;
	delete m_radioRenderer;

	delete m_levelAudioManager;
	delete m_player;
	delete m_mapManager;
}

void GamePlayScene::Init(Tmpl8::Game* p_game, Surface* p_screen)
{
	m_game = p_game;
	m_screen = p_screen;
	m_levelAudioManager = new LevelAudioManager();
	m_mapManager = new MapManager({3, 5}, "assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);

	m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
	m_mapManager->SetNewRoom({ 1, 4 });

	m_player = new Player({ ROOM_SIZE_IN_PIXELS / 2,ROOM_SIZE_IN_PIXELS / 1.5f }, p_screen, m_mapManager, false, 0);
	m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
	m_textRenderer = new TextRenderer(p_screen);
	m_radioRenderer = new RadioRenderer(p_screen, m_player, m_textRenderer);
	m_inventoryRenderer = new InventoryRenderer(m_player, m_textRenderer, m_screen);
	m_generalUIRenderer = new GeneralUIRenderer(m_screen, m_enemyManager, m_textRenderer, m_mapManager, m_player);
	m_collisionManager = new CollisionManager(m_enemyManager, m_player, m_mapManager, p_screen);
	m_saveLoadmanager = new SaveLoadManager();

}

void GamePlayScene::Init(Tmpl8::Game* p_game, Surface* p_screen, levelIndex p_levelIndex, float2 playerPos)
{
	m_game = p_game;
	m_screen = p_screen;
	m_levelAudioManager = new LevelAudioManager();

	switch (p_levelIndex)
	{
		case GamePlayScene::levelIndex::levelOneStart:
		default:
		{
			m_mapManager = new MapManager({ 3, 5 }, "assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_player = new Player(playerPos, p_screen, m_mapManager, false, 0);
			m_mapManager->SetNewRoom({ 1, 4 });
			m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
			m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
			break;
		}
		case GamePlayScene::levelIndex::levelOneFromTopL:
		{
			m_mapManager = new MapManager({ 3, 5 }, "assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_mapManager->SetNewRoom({ 0, 0 });
			m_player = new Player(playerPos, p_screen, m_mapManager, false, 0);
			m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
			m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
			break;
		}
		case GamePlayScene::levelIndex::levelOneFromTopR:
		{

			m_mapManager = new MapManager({ 3, 5 }, "assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
			m_mapManager->SetNewRoom({ 2, 0 });

			m_player = new Player(playerPos, p_screen, m_mapManager, false, 1);
			m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
			break;
		}
		case GamePlayScene::levelIndex::levelTwoFromBottomL:
		{
			m_mapManager = new MapManager({ 4, 4 }, "assets/mg2mapInsideRework_renderLayer.csv", "assets/mg2mapInsideRework2_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_mapManager->SetNewRoom({ 0, 3 });
			m_player = new Player(playerPos, p_screen, m_mapManager, false, 1);
			m_mapManager->InitTileSheet({ 154, 154 }, "assets/tiled4.png", p_screen);
			m_enemyManager = new EnemyManager("assets/mg2mapInsideRework2_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);


			break;
		}
		case GamePlayScene::levelIndex::levelTwoFromBottomR:
		{
			m_mapManager = new MapManager({ 4, 4 }, "assets/mg2mapInsideRework_renderLayer.csv", "assets/mg2mapInsideRework2_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_mapManager->SetNewRoom({ 2, 3 });
			m_mapManager->InitTileSheet({ 154, 154 }, "assets/tiled4.png", p_screen);
			m_player = new Player(playerPos, p_screen, m_mapManager, false, 1);
			m_enemyManager = new EnemyManager("assets/mg2mapInsideRework2_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);

			break;
		}
	}


	m_textRenderer = new TextRenderer(p_screen);
	m_radioRenderer = new RadioRenderer(p_screen, m_player, m_textRenderer);
	m_inventoryRenderer = new InventoryRenderer(m_player, m_textRenderer, m_screen);
	m_generalUIRenderer = new GeneralUIRenderer(m_screen, m_enemyManager, m_textRenderer, m_mapManager, m_player);
	m_collisionManager = new CollisionManager(m_enemyManager, m_player, m_mapManager, p_screen);
	m_saveLoadmanager = new SaveLoadManager();

}

void GamePlayScene::Init(Tmpl8::Game* p_game, Surface* p_screen, levelIndex p_levelIndex, float2 playerPos, Tmpl8::Game::PlayerLoadData p_loadData)
{
	m_game = p_game;
	m_screen = p_screen;
	m_levelAudioManager = new LevelAudioManager();

	switch (p_levelIndex)
	{
		case GamePlayScene::levelIndex::levelOneStart:
		default:
		{
			m_mapManager = new MapManager({3, 5},"assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_player = new Player(playerPos, p_screen, m_mapManager, false, 0);
			//m_player->SetPlayer(p_loadData);
			m_mapManager->SetNewRoom({ 1, 4 });
			m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
			m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
			break;
		}
		case GamePlayScene::levelIndex::levelOneFromTopL:
		{
			m_mapManager = new MapManager({ 3, 5 }, "assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_mapManager->SetNewRoom({ 0, 0 });
			m_player = new Player(playerPos, p_screen, m_mapManager, false, 0);
			//m_player->SetPlayer(p_loadData);
			m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
			m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
			break;
		}
		case GamePlayScene::levelIndex::levelOneFromTopR:
		{

			m_mapManager = new MapManager({ 3, 5 }, "assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
			m_mapManager->SetNewRoom({ 2, 0 });

			m_player = new Player(playerPos, p_screen, m_mapManager, false, 0);
			//m_player->SetPlayer(p_loadData);
			m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
			break;
		}
		case GamePlayScene::levelIndex::levelTwoFromBottomL:
		{
			m_mapManager = new MapManager({ 4, 4 }, "assets/mg2mapInsideRework_renderLayer.csv", "assets/mg2mapInsideRework2_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_mapManager->SetNewRoom({ 0, 3 });
			m_player = new Player(playerPos, p_screen, m_mapManager, false, 1);
			//m_player->SetPlayer(p_loadData);
			m_mapManager->InitTileSheet({ 154, 154 }, "assets/tiled4.png", p_screen);
			m_enemyManager = new EnemyManager("assets/mg2mapInsideRework2_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);


			break;
		}
		case GamePlayScene::levelIndex::levelTwoFromBottomR:
		{
			m_mapManager = new MapManager({ 4, 4 }, "assets/mg2mapInsideRework_renderLayer.csv", "assets/mg2mapInsideRework2_collisionAndNodes.csv", m_levelAudioManager, p_screen);
			m_mapManager->SetNewRoom({ 2, 3 });
			m_mapManager->InitTileSheet({ 154, 154 }, "assets/tiled4.png", p_screen);
			m_player = new Player(playerPos, p_screen, m_mapManager, false, 1);
			m_enemyManager = new EnemyManager("assets/mg2mapInsideRework2_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);

			break;
		}
	}

	m_collisionManager = new CollisionManager(m_enemyManager, m_player, m_mapManager, p_screen);
	m_player->SetPlayer(p_loadData, m_collisionManager->GetPickUpManager());

	m_textRenderer = new TextRenderer(p_screen);
	m_radioRenderer = new RadioRenderer(p_screen, m_player, m_textRenderer);
	m_inventoryRenderer = new InventoryRenderer(m_player, m_textRenderer, m_screen);
	m_generalUIRenderer = new GeneralUIRenderer(m_screen, m_enemyManager, m_textRenderer, m_mapManager, m_player);
	m_saveLoadmanager = new SaveLoadManager();

}

void GamePlayScene::Init(Tmpl8::Game* p_game, Surface* p_screen, levelIndex p_levelIndex, float2 playerPos, int2 roomIndex, Tmpl8::Game::PlayerLoadData p_loadData)
{
	m_game = p_game;
	m_screen = p_screen;
	m_levelAudioManager = new LevelAudioManager();

	switch (p_levelIndex)
	{
	case GamePlayScene::levelIndex::levelOneStart:
	default:
	{
		m_mapManager = new MapManager({ 3, 5 }, "assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);
		m_player = new Player(playerPos, p_screen, m_mapManager, false, 0);
		//m_player->SetPlayer(p_loadData);
		m_mapManager->SetNewRoom(roomIndex);
		m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
		m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
		break;
	}
	case GamePlayScene::levelIndex::levelOneFromTopL:
	{
		m_mapManager = new MapManager({ 3, 5 }, "assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);
		m_mapManager->SetNewRoom(roomIndex);
		m_player = new Player(playerPos, p_screen, m_mapManager, false, 0);
		//m_player->SetPlayer(p_loadData);
		m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
		m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
		break;
	}
	case GamePlayScene::levelIndex::levelOneFromTopR:
	{

		m_mapManager = new MapManager({ 3, 5 }, "assets/mg2MapOutside_withRooms_render.csv", "assets/mg2MapOutside_withRooms3_collisionAndNodes.csv", m_levelAudioManager, p_screen);
		m_mapManager->InitTileSheet({ 57, 57 }, "assets/tiles.png", p_screen);
		m_mapManager->SetNewRoom(roomIndex);

		m_player = new Player(playerPos, p_screen, m_mapManager, false, 0);
		//m_player->SetPlayer(p_loadData);
		m_enemyManager = new EnemyManager("assets/mg2MapOutside_withpathfinding_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);
		break;
	}
	case GamePlayScene::levelIndex::levelTwoFromBottomL:
	{
		m_mapManager = new MapManager({ 4, 4 }, "assets/mg2mapInsideRework_renderLayer.csv", "assets/mg2mapInsideRework2_collisionAndNodes.csv", m_levelAudioManager, p_screen);
		m_mapManager->SetNewRoom(roomIndex);
		m_player = new Player(playerPos, p_screen, m_mapManager, false, 1);
		//m_player->SetPlayer(p_loadData);
		m_mapManager->InitTileSheet({ 154, 154 }, "assets/tiled4.png", p_screen);
		m_enemyManager = new EnemyManager("assets/mg2mapInsideRework2_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);


		break;
	}
	case GamePlayScene::levelIndex::levelTwoFromBottomR:
	{
		m_mapManager = new MapManager({ 4, 4 }, "assets/mg2mapInsideRework_renderLayer.csv", "assets/mg2mapInsideRework2_collisionAndNodes.csv", m_levelAudioManager, p_screen);
		m_mapManager->SetNewRoom(roomIndex);
		m_mapManager->InitTileSheet({ 154, 154 }, "assets/tiled4.png", p_screen);
		m_player = new Player(playerPos, p_screen, m_mapManager, false, 1);
		m_enemyManager = new EnemyManager("assets/mg2mapInsideRework2_guardinstructions.csv", m_player->GetCamPos(), m_player->GetPLayerWorldPos(), p_screen, m_mapManager, m_levelAudioManager);

		break;
	}
	}

	m_collisionManager = new CollisionManager(m_enemyManager, m_player, m_mapManager, p_screen);
	m_player->SetPlayer(p_loadData, m_collisionManager->GetPickUpManager());

	m_textRenderer = new TextRenderer(p_screen);
	m_radioRenderer = new RadioRenderer(p_screen, m_player, m_textRenderer);
	m_inventoryRenderer = new InventoryRenderer(m_player, m_textRenderer, m_screen);
	m_generalUIRenderer = new GeneralUIRenderer(m_screen, m_enemyManager, m_textRenderer, m_mapManager, m_player);
	m_saveLoadmanager = new SaveLoadManager();
}

void GamePlayScene::Update(float p_deltaTime)
{
	const float2& playerPos = m_player->GetPLayerWorldPos();
	
	if (playerPos.y < 0)
	{
		Tmpl8::Game::PlayerLoadData saveData = m_player->GetPlayerData();
		saveData.levelIndex++;
		m_game->ChangeScene(Tmpl8::Game::level::levelTwo, *m_player->GetCamPos(), saveData);
		return;
	}
	else if ((playerPos.y + 1) > static_cast<float>(m_mapManager->GetMapRoomSize().y * ROOM_SIZE))
	{
		Tmpl8::Game::PlayerLoadData saveData = m_player->GetPlayerData();
		saveData.levelIndex--;
		m_game->ChangeScene(Tmpl8::Game::level::levelOne, *m_player->GetCamPos(), saveData);
		return;
	}


	m_screen->Clear(0);
	m_levelAudioManager->Update(p_deltaTime);
	m_generalUIRenderer->Render();

	if (m_player->CheckIfPlayerIsDead())
	{
		m_game->ChangeScene(Tmpl8::Game::Scenes::loseScreen);
		return;
	}

	switch (m_currentRenderState)
	{
		case RenderState::gameplay:
		{
			m_mapManager->Render();
			m_enemyManager->Update(p_deltaTime);
			m_player->Update(p_deltaTime);
			m_player->Render();

			float2 playerCurrentPos = m_player->GetScreenPos();
			int2 playerPosInRoom = { static_cast<int>(playerCurrentPos.x) / TILE_SIZE, static_cast<int>(playerCurrentPos.y) / TILE_SIZE };
			
			if (m_player->PlayerIsCrouching())
			{
				m_mapManager->RenderSecondLayer(playerPosInRoom, m_player->PlayerIsCrouching());
			}
			m_collisionManager->Render();
			m_collisionManager->Update(p_deltaTime);

			break;
		}
		case RenderState::equipWeapon:
		case RenderState::equipItem:
		{
			m_inventoryRenderer->Render();
			break;
		}
		case RenderState::radio:
		{

			m_radioRenderer->Render();
			m_radioRenderer->Update(p_deltaTime);
			break;
		}
		default:
		break;
	}

	if (m_saveTextTimer > 0)
	{
		m_saveTextTimer -= p_deltaTime;
		m_textRenderer->RenderText({ 70, 70 }, { 30, 30 }, "gamestate saved");
	}

	if (m_player->PlayerWon())
	{
		m_game->ChangeScene(Tmpl8::Game::Scenes::winScreen);
	}

}

void GamePlayScene::KeyUp(int p_keyUp)
{
	m_player->KeyUp(p_keyUp);
}

void GamePlayScene::KeyDown(int p_keyDown)
{

	switch (m_currentRenderState)
	{
		case GamePlayScene::RenderState::gameplay:
		{
			m_player->KeyDown(p_keyDown);
			break;
		}
		case GamePlayScene::RenderState::equipWeapon:
		{
			m_player->KeyDownInventory(p_keyDown, true);
			break;
		}
		case GamePlayScene::RenderState::equipItem:
		{
			m_player->KeyDownInventory(p_keyDown, false);
			break;
		}
		case GamePlayScene::RenderState::radio:
		{
			m_player->KeyDownRadio(p_keyDown);
			break;
		}
		default:
		break;
	}


	switch (p_keyDown)
	{
		// show inventory for weapon
		case GLFW_KEY_1:
		{
			m_currentRenderState = (m_currentRenderState == RenderState::equipWeapon) ? m_currentRenderState = RenderState::gameplay : RenderState::equipWeapon;
			m_inventoryRenderer->SetData(true);
			break;
		}
		// show inventory for items
		case GLFW_KEY_2:
		{
			m_currentRenderState = (m_currentRenderState == RenderState::equipItem) ? m_currentRenderState = RenderState::gameplay : RenderState::equipItem;
			m_inventoryRenderer->SetData(false);
			break;
		}
		// show radio
		case GLFW_KEY_3:
		{
			m_currentRenderState = (m_currentRenderState == RenderState::radio) ? m_currentRenderState = RenderState::gameplay : RenderState::radio;
			break;
		}
		// save state
		case GLFW_KEY_4:
		{
			m_saveTextTimer = 1500;
			m_saveLoadmanager->SaveGameState(m_player->GetPlayerData());
			break;
		}
		default:
		break;
	}
}
