#include "precomp.h"
#include "generalUIRenderer.h"
#include <renderingConst.h>
#include "EnemyManager.h"
#include "TextRenderer.h"
#include "Player.h"
#include "AllEquippableHeader.h"

GeneralUIRenderer::GeneralUIRenderer(Surface* p_ptrScreen, EnemyManager* p_enemyManager, TextRenderer* p_textRenderer, MapManager* p_mapManager, Player* p_player)
{
	m_ptrScreen = p_ptrScreen;
	m_enemyManager = p_enemyManager;
	m_textRenderer = p_textRenderer;
	m_mapManager = p_mapManager;
	m_player = p_player;

	m_itemBannerSpritesheet = SpriteSheet({ 1,1 }, "assets/itemBanner.png", p_ptrScreen);
	m_callSprite = SpriteSheet({ 1,1 }, "assets/callSprite.png", p_ptrScreen);

}

void GeneralUIRenderer::Render()
{

	m_itemBannerSpritesheet.renderFromScaled({ 0,0 }, { 1000, 500 }, { 350, 230 });
	m_itemBannerSpritesheet.renderFromScaled({ 0,0 }, { 1000, 750 }, { 350, 230 });

	const baseWeapon* renderedWeapon = m_player->LookAtCurrentEquipedWeapon();
	if (renderedWeapon != nullptr)
	{
		renderedWeapon->RenderIcon({ 1100, 540 }, { 175, 100 });
		m_textRenderer->RenderText({ SCRWIDTH - 200, 680 }, {30, 30}, static_cast<char>((renderedWeapon->m_ammo / 10)) + '0');
		m_textRenderer->RenderText({ SCRWIDTH - 160, 680 }, {30, 30}, static_cast<char>((renderedWeapon->m_ammo % 10)) + '0');

	}


	EnemyManager::GameplayState gameplayState = m_enemyManager->LookAtCurrentGameState();

	m_ptrScreen->Bar(static_cast<int>(RADAR_RENDER_START_X), 100, 1350, 450, DARK_GREEN);

	for (int i = 0; i < 3; i++)
	{
		m_ptrScreen->Box(100 + i, SCRHEIGHT + i - 250, 300 + i, SCRHEIGHT + i - 220, 0xffffff);
	}

	int healthRenderStart = 103;
	float healthBarSize = static_cast<float>(194.0f * m_player->LookAtHealthpercentage());
	m_ptrScreen->Bar(healthRenderStart, SCRHEIGHT - 250, healthRenderStart + static_cast<int>(healthBarSize), SCRHEIGHT - 223, 0xff0000);

	//m_radioflags
	if (m_player->ShowRadioFLags())
	{
		m_callSprite.renderFromScaled({ 0,0 }, { 350, SCRHEIGHT - 250 }, { 150, 40 });
	}



	for (MapManager::RoomStruct*& room : adjacentRooms) {room = nullptr;}

	switch (gameplayState)
	{
		case EnemyManager::GameplayState::normal:
		case EnemyManager::GameplayState::caution:
		{
			int2 mapSize = m_mapManager->GetMapRoomSize();

			float2 worldPos = m_player->GetPLayerWorldPos() / ROOM_SIZE;
			int2 currentRoom = { static_cast<int>(worldPos.x) , static_cast<int>(worldPos.y) };

			for (int j = currentRoom.y - 1; j <= currentRoom.y + 1; j++) {
				for (int i = currentRoom.x - 1; i <= currentRoom.x + 1; i++) {
					if (i >= 0 && i < mapSize.x &&   // Check x bounds
						j >= 0 && j < mapSize.y)  // Check y bounds
					{
						// Calculate the correct index for the 3x3 `adjacentRooms` array
						int adjacentIndex = (i - currentRoom.x + 1) + (j - currentRoom.y + 1) * 3;
						adjacentRooms[adjacentIndex] = &m_mapManager->rooms->data[i + j * mapSize.x];
					}
				}
			}
			int boxsize = 350 / 3;

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					int2 renderPos = { static_cast<int>(RADAR_RENDER_START_X + (i * boxsize)), 100 + (j * boxsize)};
					if (adjacentRooms[i + j * 3] != nullptr)
					{
						RenderRaderTile(renderPos, { boxsize , boxsize }, adjacentRooms[i + j * 3]->collisionMask);
					}
					else
					{
						m_ptrScreen->Bar(renderPos.x, renderPos.y, renderPos.x + boxsize, renderPos.y + boxsize, 0x0);
					}

				}
			}

			for (int i = 0; i < 3; i++)
			{
				m_ptrScreen->Box(static_cast<int>(RADAR_RENDER_START_X) + i, 100 + i, 1350 + i, 450 + i, LIGHT_GREEN);
				float lineA = 350 / 3;
				float lineB = (350 / 3) * 2;

				//vert lines
				m_ptrScreen->Line(static_cast<float>(RADAR_RENDER_START_X + lineA + i), static_cast<float>(100 + i), static_cast<float>(RADAR_RENDER_START_X + lineA + i), static_cast<float>(450 + i), LIGHT_GREEN);
				m_ptrScreen->Line(static_cast<float>(RADAR_RENDER_START_X + lineB + i), static_cast<float>(100 + i), static_cast<float>(RADAR_RENDER_START_X + lineB + i), static_cast<float>(450 + i), LIGHT_GREEN);

				//hoz lines
				m_ptrScreen->Line(static_cast<float>(RADAR_RENDER_START_X + i), static_cast<float>(100 + lineA + i), static_cast<float>(1350 + i), static_cast<float>(100 + lineA + i), LIGHT_GREEN);
				m_ptrScreen->Line(static_cast<float>(RADAR_RENDER_START_X + i), static_cast<float>(100 + lineB + i), static_cast<float>(1350 + i), static_cast<float>(100 + lineB + i), LIGHT_GREEN);
			}

			const int2 camPos = currentRoom * ROOM_SIZE_IN_PIXELS;


			for (int i = 0; i < m_enemyManager->m_ptrArrayEnemies->size; i++)
			{
				if (!m_enemyManager->m_ptrArrayEnemies->data[i]->IsAlive()) continue;

				float2 pos = m_enemyManager->m_ptrArrayEnemies->data[i]->GetPos();

				if (pos.x > (camPos.x - ((ROOM_SIZE_IN_PIXELS))) && pos.x < (camPos.x + (ROOM_SIZE_IN_PIXELS * 2)) &&
					pos.y >(camPos.y - ROOM_SIZE_IN_PIXELS) && pos.y < (camPos.y + (ROOM_SIZE_IN_PIXELS * 2)))
				{

					// the extra ROOM_SIZE_IN_PIXELS+ is because the start of the read index is negative
					pos = (pos - camPos) + ROOM_SIZE_IN_PIXELS;

					// devide by the grid (3 * 3) grid
					pos /= (ROOM_SIZE_IN_PIXELS * 3);

					int2 renderPos = { static_cast<int>(RADAR_RENDER_START_X + (pos.x * 350)),
				   static_cast<int>(100 + (pos.y * 350)) };

					//int2 renderPos = { static_cast<int>(RADAR_RENDER_START_X + (pos.x * 350)), static_cast<int>((100 + (pos.y * 350))) };
					m_ptrScreen->Bar(renderPos.x - 4, renderPos.y - 4, renderPos.x + 4, renderPos.y + 4, 0xffffff);
				}

			}

			//int2 playerPos = { static_cast<int>(m_player->GetPLayerWorldPos().x) % ROOM_SIZE_IN_PIXELS, static_cast<int>(m_player->GetPLayerWorldPos().y) % ROOM_SIZE_IN_PIXELS };
			float2 playerPos = (m_player->GetScreenPos()) / ROOM_SIZE_IN_PIXELS;

			
			int2 playerRenderPos = { static_cast<int>(RADAR_RENDER_START_X + boxsize + (boxsize * playerPos.x)), static_cast<int>(100 + boxsize + (boxsize * playerPos.y)) };
			m_ptrScreen->Bar(playerRenderPos.x - 4, playerRenderPos.y - 4, playerRenderPos.x + 4, playerRenderPos.y + 4, 0xff0000);



			break;
		}
		case EnemyManager::GameplayState::alert:
		{
			m_textRenderer->RenderText({ static_cast<int>(RADAR_RENDER_START_X + 80), 150}, {40, 40}, "alert");
			int timer = static_cast<int>(m_enemyManager->LootAtTimer());

			char buffer[6] = "0000";
			buffer[0] = (timer / 1000) % 10 + '0';  
			buffer[1] = (timer / 100) % 10 + '0'; 
			buffer[2] = '.';
			buffer[3] = (timer / 10) % 10 + '0';    
			buffer[4] =  timer % 10 + '0';           

			m_textRenderer->RenderText({ static_cast<int>(RADAR_RENDER_START_X + 105), 250 }, { 30, 30 }, buffer);

			m_ptrScreen->Box(static_cast<int>(RADAR_RENDER_START_X + 100), 230, static_cast<int>(RADAR_RENDER_START_X + 255), 295, 0xff0000);

			break;
		}
		case EnemyManager::GameplayState::evasion:
		{
			m_textRenderer->RenderText({ static_cast<int>(RADAR_RENDER_START_X + 30), 150 }, { 40, 40 }, "evasion");
			int timer = static_cast<int>(m_enemyManager->LootAtTimer());

			char buffer[6] = "0000";
			buffer[0] = (timer / 1000) % 10 + '0';
			buffer[1] = (timer / 100) % 10 + '0';
			buffer[2] = '.';
			buffer[3] = (timer / 10) % 10 + '0';
			buffer[4] = timer % 10 + '0';

			m_textRenderer->RenderText({ static_cast<int>(RADAR_RENDER_START_X + 105), 250 }, { 30, 30 }, buffer);
			m_ptrScreen->Box(static_cast<int>(RADAR_RENDER_START_X + 100), 230, static_cast<int>(RADAR_RENDER_START_X + 255), 295, 0xcc692b);

			break;
		}
		default:
		break;
	}





}

void GeneralUIRenderer::RenderRaderTile(int2 p_pos, int2 p_size, Surface* p_mask)
{
	uint* screenPixels = m_ptrScreen->pixels;
	uint* maskPixels = p_mask->pixels;

	float2 scale = { static_cast<float>(p_mask->width) / static_cast<float>(p_size.x), static_cast<float>(p_mask->height) / static_cast<float>(p_size.y) };

	for (int i = 0; i < p_size.x; i++)
	{
		for (int j = 0; j < p_size.y; j++)
		{
			int scaledIndex = static_cast<int>(i * scale.x) + static_cast<int>(j * scale.y) * p_mask->width;

			uint currentPixel = maskPixels[scaledIndex];

			if ((currentPixel & TRANSPARENCY_MASK) == TRANSPARENCY_MASK)
			{
				screenPixels[(p_pos.x + i) + (p_pos.y + j) * m_ptrScreen->width] = 0x0;
			}

		}
	}
}
