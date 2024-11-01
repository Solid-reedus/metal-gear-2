#include "precomp.h"
#include "Player.h"

#include "BaseActor.h"
#include "SpriteSheet.h"
#include "game.h"
#include "renderingConst.h"
#include "projectile.h"

#include "AllEquippableHeader.h"
#include "basePickupable.h"
#include "radioManager.h"


Player::Player(float2 p_startPos, Surface* p_ptrScreen, MapManager* p_ptrMapManager, bool p_playIntro, int p_levelIndex)
{
	m_health = 600;
	m_animIndex = 0;
	m_pos = p_startPos;
	//m_worldPos = p_startPos;
	m_dir = { 0,0 };

	m_lookingDir = dirUp;
	m_walkDir = dirNone;

	m_levelIndex = p_levelIndex;

	m_ptrSpritesheet = new SpriteSheet(SPRITESHEET_SIZE, "assets/solid_snake_spritesheet_cyan_background2.png", p_ptrScreen);
	m_ptrMapManager = p_ptrMapManager;

	m_radioManager = new RadioManager();

	m_roomSize = m_ptrMapManager->GetRoomSize();
	m_roomSize *= TILE_SIZE;

	m_camPos = m_ptrMapManager->GetCurrentRoom();
	m_camPos *= ROOM_SIZE_IN_PIXELS;
	m_currentRoom = m_ptrMapManager->GetCurrentRoom();

	m_projectilePool = new DymArr<Projectile*>{ new Projectile*[10] , 10 };

	for (int i = 0; i < m_projectilePool->size; i++)
	{
		m_projectilePool->data[i] = new Projectile({ 8,8 }, 0xffffff, p_ptrScreen);
	}

	if (p_playIntro)
	{
		m_introAnimCountDown = INTRO_ANIM_DURATION;
		m_playerState |= playerIsInAnimation;
	}
	else
	{
		m_introAnimCountDown = 0;
		m_playerState |= playerInControl;
	}

	m_worldPos = (m_pos / TILE_SIZE) + (static_cast<float2>(m_ptrMapManager->GetCurrentRoom()) * ROOM_SIZE);

}

Player::~Player()
{
	for (const baseWeapon* weapon : m_weaponInventory)
	{
		if (weapon != nullptr)
		{
			delete weapon;
		}
	}
	for (const Baseitem* item : m_itemInventory)
	{
		if (item != nullptr)
		{
			delete item;
		}
	}

	for (int i = 0; i < m_projectilePool->size; i++)
	{
		if (m_projectilePool->data[i] != nullptr)
		{
			delete m_projectilePool->data[i];
		}
	}
}

void Player::Update(float p_deltaTime)
{
	float2 nextStep = { 0,0 };
	if ((m_playerState & playerCrouching) == playerCrouching) nextStep = m_dir * (PLAYER_SPEED * p_deltaTime / 3);
	else nextStep = m_dir * PLAYER_SPEED * p_deltaTime;

	float2 offsetStep = m_pos;
	offsetStep.y += 20;
	offsetStep.x -= 10;
	float2 offsetStep2 = offsetStep;
	offsetStep2.x += 25;

	if (NextStepIsValid(offsetStep, offsetStep + nextStep) &&
		NextStepIsValid(offsetStep2, offsetStep2 + nextStep) &&
		(m_playerState & playerInControl) == playerInControl )
	{
		m_pos += nextStep;
	}

	if (m_introAnimCountDown > 0)
	{
		m_introAnimCountDown -= p_deltaTime;

		if (m_introAnimCountDown <= 0)
		{
			m_playerState |= playerInControl;
			m_playerState ^= playerIsInAnimation;
		}
	}

	if (m_punchCoolDown != 0)
	{
		m_punchCoolDown += p_deltaTime;
		if (m_punchCoolDown > PUNCH_COOL_DUR)
		{
			m_punchCoolDown = 0;
		}
	}


	// check if next step is on a teleport marker
	CheckTeleportPlayer();


	m_animIndex += p_deltaTime / 200;


	if (m_worldPos.y < (TILE_SIZE / 2) || (m_worldPos.y + 1) > static_cast<float>(m_ptrMapManager->GetMapRoomSize().y * ROOM_SIZE))
	{
		m_worldPos = (m_pos / TILE_SIZE) + (static_cast<float2>(m_ptrMapManager->GetCurrentRoom()) * ROOM_SIZE);
		return;
	}

	// the extra TILE_SIZE is for when the left wall is at the very left and
	// and can cause the player to clip into the wall
	if (m_pos.x < MAP_RENDERING_START - TILE_SIZE)
	{
		m_pos.x = static_cast<float>(m_roomSize.x);
		m_ptrMapManager->IncrementToNewRoom({ -1,0 });
		m_currentRoom = m_ptrMapManager->GetCurrentRoom();
		ChangeCamPos();
	}
	else if (m_pos.y < MAP_RENDERING_START)
	{
		m_pos.y = static_cast<float>(m_roomSize.y);
		m_ptrMapManager->IncrementToNewRoom({ 0,-1 });
		m_currentRoom = m_ptrMapManager->GetCurrentRoom();
		ChangeCamPos();
	}
	else if (m_pos.x > ROOM_SIZE_IN_PIXELS)
	{
		m_pos.x = static_cast<float>(MAP_RENDERING_START);
		m_ptrMapManager->IncrementToNewRoom({ 1, 0 });
		m_currentRoom = m_ptrMapManager->GetCurrentRoom();
		ChangeCamPos();
	}
	else if (m_pos.y > ROOM_SIZE_IN_PIXELS)
	{
		m_pos.y = static_cast<float>(MAP_RENDERING_START);
		m_ptrMapManager->IncrementToNewRoom({ 0, 1 });
		m_currentRoom = m_ptrMapManager->GetCurrentRoom();
		ChangeCamPos();
	}


	if (!m_isInRoom)
	{
		m_worldPos = (m_pos / TILE_SIZE) + (static_cast<float2>(m_ptrMapManager->GetCurrentRoom()) * ROOM_SIZE);
	}

	if (m_levelIndex == 1 && m_currentRoom.x == 1 && m_currentRoom.y == 1)
	{
		RectInt rect = { MAP_RENDERING_START + (TILE_SIZE * 12), MAP_RENDERING_START + (TILE_SIZE * 38), 150, 50 };
		if (rect.Overlaps(GetRectFromPlayer()))
		{
			m_playerWon = true;
		}
	}


}

void Player::KeyDown(int p_keyDown)
{

	switch (p_keyDown)
	{
		case GLFW_KEY_W:
		{
			m_playerState |= playerIsWalking;
			m_lookingDir = dirUp;
			m_walkDir = dirUp;
			m_dir = { 0, -1 };
			break;
		}
		case GLFW_KEY_D:
		{
			m_playerState |= playerIsWalking;
			m_lookingDir = dirRight;
			m_walkDir = dirRight;
			m_dir = { 1, 0 };
			break;
		}
		case GLFW_KEY_S:
		{
			m_playerState |= playerIsWalking;
			m_lookingDir = dirDown;
			m_walkDir = dirDown;
			m_dir = { 0, 1 };
			break;
		}
		case GLFW_KEY_A:
		{
			m_playerState |= playerIsWalking;
			m_lookingDir = dirLeft;
			m_walkDir = dirLeft;
			m_dir = { -1, 0 };
			break;
		}
		case GLFW_KEY_C:
		{
			// invert the crouching flag
			m_playerState& playerCrouching ? m_playerState ^= playerCrouching : m_playerState |= playerCrouching;
			int2 currentPos = { static_cast<int>(m_pos.x / TILE_SIZE), static_cast<int>(m_pos.y / TILE_SIZE) };


			// if the player tries to stand up in a crouch only area then he will automaticly be 
			// forced to crouch
			if (m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].crouchAreaCount > 0)
			{
				int2* crouchAreas = m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].ptrArrayCrouchAreas;
				for (int i = 0; i < m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].crouchAreaCount; i++)
				{
					if (crouchAreas[i] == currentPos)
					{
						m_playerState |= playerCrouching;
						break;
					}
				}
			}

			if (m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].visibleCrouchAreaCount > 0)
			{
				int2* crouchAreas = m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].ptrArrayVisibleCrouchAreas;
				
				for (int i = 0; i < m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].visibleCrouchAreaCount; i++)
				{
					if (crouchAreas[i] == currentPos)
					{
						m_playerState |= playerCrouching;
						break;
					}
				}
			}

			break;
		}
		//punch
		case GLFW_KEY_U:
		{

			if (m_punchCoolDown == 0 && (m_playerState & playerCrouching) != playerCrouching)
			{
				m_punchCoolDown++;
			}

			break;
		}
		//use weapon
		case GLFW_KEY_I:
		{
			if (m_currentWeapon != nullptr && m_currentWeapon->m_ammo > 0 && (m_playerState & playerCrouching) != playerCrouching)
			{
				m_currentWeapon->m_ammo--;
				float2 bulletDir = {0,0};
				switch (m_lookingDir)
				{
					case BaseActor::dirUp:
					{
						bulletDir = { 0,-1 };
						break;
					}
					case BaseActor::dirRight:
					{
						bulletDir = { 1,0 };
						break;
					}
					case BaseActor::dirDown:
					{
						bulletDir = { 0,1 };
						break;
					}
					case BaseActor::dirLeft:
					{
						bulletDir = { -1,0 };
						break;
					}
					case BaseActor::dirNone:
					default:
					break;
				}

				Projectile* usedProj = m_projectilePool->data[projectileIndex];
				float2 bulletStart = { m_pos.x + (PLAYER_SIZE.x / 2 + (bulletDir.x * 30)), m_pos.y + (PLAYER_SIZE.y / 3 + (bulletDir.y * 30))};

				m_currentWeapon->Attack(bulletStart, bulletDir, usedProj);
				projectileIndex++;
				if (projectileIndex >= m_projectilePool->size) {projectileIndex = 0;}

			}
			break;
		}

		default:
		break;
	}

}

void Player::KeyUp(int p_keyDown)
{

	if (m_walkDir == p_keyDown)
	{
		m_walkDir = dirNone;
		m_playerState ^= playerIsWalking;
		m_dir = { 0, 0 };
	}

}

void Player::KeyDownRadio(int p_keyDown)
{

	switch (p_keyDown)
	{
		case GLFW_KEY_W:
		{
			if (m_radioIndex > 98)
			{
				m_radioIndex = 0;
			}
			else
			{
				m_radioIndex++;
			}
			break;
		}
		case GLFW_KEY_S:
		{

			if (m_radioIndex < 2)
			{
				m_radioIndex = 99;
			}
			else
			{
				m_radioIndex--;
			}
			break;
		}
		case GLFW_KEY_U:
		{

			if (m_currentConversation != nullptr)
			{
				//DymArr<voiceLine>
				conversationIndex++;
				if (conversationIndex >= m_currentConversation->voiceLines->size)
				{
					conversationIndex = 0;
					m_currentConversation = nullptr;
				}

				//do stuff
				break;
			}

			if (m_radioIndex == 85)
			{
				int2 roomPos = m_ptrMapManager->GetCurrentRoom();
				if (m_levelIndex == 0 && roomPos.x == 2 && roomPos.y == 2 && (m_radioflags & fenceRing) == fenceRing)
				{
					m_currentConversation = m_radioManager->GetConversation(RadioManager::conversations::campelCrouchInfo);
				}
				else
				{
					m_currentConversation = m_radioManager->GetConversation(RadioManager::conversations::campelDefault);
				}
			}
			else
			{
				m_currentConversation = m_radioManager->GetConversation(RadioManager::conversations::noResponse);
			}
			break;
		}
		default:
		break;
	}

	if (m_levelIndex == 0 && m_currentRoom.x == 2 && m_currentRoom.y == 2)
	{
		m_radioflags &= ~fenceRing;
	}
	else if (m_levelIndex == 1 && m_currentRoom.x == 3 && m_currentRoom.y == 3)
	{
		m_radioflags &= ~insideRing;
	}

}

void Player::KeyDownInventory(int p_keyDown, bool p_isInWeaponSelect)
{
	switch (p_keyDown)
	{
		case GLFW_KEY_W:
		{
			if (m_inventoryIndex.y > 0)
			{
				m_inventoryIndex.y--;
				return;
			}
			break;
		}
		case GLFW_KEY_D:
		{
			if (m_inventoryIndex.x < PLAYER_INVENTORY_MAX_INDEX.x)
			{
				m_inventoryIndex.x++;
				return;
			}
			break;
		}
		case GLFW_KEY_S:
		{

			if (m_inventoryIndex.y < PLAYER_INVENTORY_MAX_INDEX.y)
			{
				m_inventoryIndex.y++;
				return;
			}
			break;
		}
		case GLFW_KEY_A:
		{
			if (m_inventoryIndex.x > 0)
			{
				m_inventoryIndex.x--;
				return;
			}
			break;
		}
		case GLFW_KEY_U:
		{
			if (p_isInWeaponSelect)
			{
				baseWeapon* newWeapon = m_weaponInventory[m_inventoryIndex.x * (PLAYER_INVENTORY_MAX_INDEX.x + 1) + m_inventoryIndex.y];
				
				if (m_currentWeapon == newWeapon || newWeapon == nullptr)
				{
					// specifically this bitwise opr was generated by chatgpt 
					m_playerState &= ~playerHasGun;
					m_currentWeapon = nullptr;
				}
				else
				{
					m_playerState |= playerHasGun;
					m_currentWeapon = newWeapon;
				}
				
			}
			else
			{
				// still needs code
			}
			break;
		}
		default:
		break;
	}
}

const float2 Player::GetScreenPos()
{
	if (!(m_playerState & playerIsInAnimation))
	{
		return m_pos;
	}
	else
	{
		float2 renderPos = m_pos;
		float posX = (ROOM_SIZE_IN_PIXELS / 2 - 100) * (m_introAnimCountDown / INTRO_ANIM_DURATION);
		renderPos.x += posX;
		return renderPos;
	}
}

bool Player::ShowRadioFLags()
{
	if (m_levelIndex == 0 && m_currentRoom.x == 2 && m_currentRoom.y == 2 && (m_radioflags & fenceRing) == fenceRing)
	{
		m_radioIndex = 85;
		return true;
	}
	else if (m_levelIndex == 1 && m_currentRoom.x == 3 && m_currentRoom.y == 3 && (m_radioflags & insideRing) == insideRing)
	{
		return true;
	}

	return false;
}

DymArr<const BasePickupable*>* Player::GetRenderedItems(bool p_getWeapon) const
{
	DymArr<const BasePickupable*>* returnedArr = new DymArr<const BasePickupable*>{ new const BasePickupable * [6], 6 };

	int i = 0;
	if (p_getWeapon)
	{
		for (const baseWeapon* pickupable : m_weaponInventory)
		{
			returnedArr->data[i++] = dynamic_cast<const BasePickupable*>(pickupable);
		}
		return returnedArr;
	}
	else
	{
		for (const Baseitem* pickupable : m_itemInventory)
		{
			returnedArr->data[i++] = dynamic_cast<const BasePickupable*>(pickupable);
		}
		return returnedArr;
	}
}

const RectInt Player::GetRectFromPlayer() const
{
	int2 pos = { static_cast<int>(m_pos.x), static_cast<int>(m_pos.y) };
	return { pos.x, pos.y, PLAYER_SIZE.x, PLAYER_SIZE.y };
}

void Player::AddWeapon(baseWeapon* p_newWeapon)
{
	for (baseWeapon*& weaponSlot : m_weaponInventory)
	{
		if (weaponSlot == nullptr)
		{
			weaponSlot = p_newWeapon;
			return;
		}
	}
}

Tmpl8::Game::PlayerLoadData Player::GetPlayerData()
{
	int bulletCount = ((m_weaponInventory[0] == nullptr)) ? 0 : m_weaponInventory[0]->m_ammo;

	Tmpl8::Game::PlayerLoadData data =
	{
		{static_cast<int>(m_worldPos.x), static_cast<int>(m_worldPos.y)},
		m_levelIndex,
		m_health,
		bulletCount,
		(m_weaponInventory[0] != nullptr),
		static_cast<bool>(m_playerState & playerCrouching),
	};
	return data;
}

void Player::SetPlayer(Tmpl8::Game::PlayerLoadData p_loadData, PickupManager* p_pickupManager)
{
	m_health = p_loadData.health;
	m_levelIndex = p_loadData.levelIndex;
	if (p_loadData.hasGun)
	{
		m_weaponInventory[0] = dynamic_cast<baseWeapon*>(p_pickupManager->TransferPickable(0));
		m_weaponInventory[0]->m_ammo = p_loadData.bullets;
	}

	if (p_loadData.isCrouching)
	{
		m_playerState |= playerCrouching;
	}
}

void Player::TakeDamage(int p_amount)
{
	m_health -= p_amount;
	if (m_health < 1)
	{
		printf("player should be dead");
	}
}

const float2 Player::LookAtPlayerWalkDir() const
{

	switch (m_lookingDir)
	{
		case BaseActor::dirDown:
		{
			return { 0,1 };
		}
		case BaseActor::dirRight:
		{
			return { 1,0 };
		}
		case BaseActor::dirUp:
		{
			return { 0,-1 };
		}
		case BaseActor::dirLeft:
		{
			return { -1,0 };
		}
		case BaseActor::dirNone:
		default:
		break;
	}
	return { 0,0 };
}

const RadioManager::voiceLine* Player::LookAtCurrentVoiceLine() const
{
	if (m_currentConversation == nullptr) {return nullptr;}
	printf("%d \n", conversationIndex);
	return &m_currentConversation->voiceLines->data[conversationIndex];
}

const float Player::LookAtHealthpercentage() const
{
	if (m_health > 1)
	{
		return  static_cast<float>(m_health) / static_cast<float>(MAX_HEALTH);
	}
	else
	{
		return 0.0f;
	}
}

bool Player::NextStepIsValid(float2 p_currentPos, float2 p_nextStep) const
{
	// Calculate the difference in movement in terms of tiles
	int dif = abs(static_cast<int>((p_nextStep.x - p_currentPos.x) / ROOM_SIZE)) +
		abs(static_cast<int>((p_nextStep.y - p_currentPos.y) / ROOM_SIZE));


	if (dif <= 1)
	{
		// No spike protection needed, just check for collisions at the next step
		int2 currentTile = { static_cast<int>(p_nextStep.x / TILE_SIZE) % ROOM_SIZE, static_cast<int>(p_nextStep.y / TILE_SIZE) % ROOM_SIZE };
		int2* colliders = m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].ptrArrayColliders;

		for (int i = 0; i < m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].colliderCount; i++)
		{
			if (colliders[i].x == currentTile.x && colliders[i].y == currentTile.y)
			{
				return false;
			}
		}

		// check if there is any crouch markers in the room if not then there is no need to check
		// and if the player is crouching then there is no need to check
		if (m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].crouchAreaCount > 0 && (m_playerState & playerCrouching) != playerCrouching)
		{
			int2* crouchAreas = m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].ptrArrayCrouchAreas;

			for (int i = 0; i < m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].crouchAreaCount; i++)
			{
				if (crouchAreas[i].x == currentTile.x && crouchAreas[i].y == currentTile.y)
				{
					return false;
				}
			}
		}
	}
	else
	{
		// Handle large movements by checking intermediate steps
		float2 direction = (p_nextStep - p_currentPos) / static_cast<float>(dif);
		float2 currentPos = p_currentPos;

		for (int i = 0; i < dif; i++)
		{
			currentPos += direction;
			int2 currentTile = { static_cast<int>(currentPos.x / TILE_SIZE) % ROOM_SIZE, static_cast<int>(currentPos.y / TILE_SIZE) % ROOM_SIZE };
			int2* colliders = m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].ptrArrayColliders;

			for (size_t j = 0; j < m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].colliderCount; j++)
			{
				if (colliders[j].x == currentTile.x && colliders[j].y == currentTile.y)
				{
					return false;
				}
			}
		}

		if (m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].crouchAreaCount > 0 && (m_playerState & playerCrouching) != playerCrouching)
		{
			for (int i = 0; i < dif; i++)
			{
				currentPos += direction;
				int2 currentTile = { static_cast<int>(currentPos.x / TILE_SIZE) % ROOM_SIZE, static_cast<int>(currentPos.y / TILE_SIZE) % ROOM_SIZE };
				int2* crouchAreas = m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].ptrArrayCrouchAreas;

				for (int j = 0; j < m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].colliderCount; j++)
				{
					if (crouchAreas[j].x == currentTile.x && crouchAreas[j].y == currentTile.y)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

void Player::CheckTeleportPlayer()
{
	// if current room doesnt have markers then there is np need to check
	if (m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].teleportMakerCount < 1)
	{
		return;
	}

	int2 currentTile = { static_cast<int>(m_pos.x / TILE_SIZE) % ROOM_SIZE, static_cast<int>(m_pos.y / TILE_SIZE) % ROOM_SIZE };

	int2* teleportMarkers = m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].ptrArrayTeleportMarkers;

	for (int i = 0; i < m_ptrMapManager->rooms->data[m_ptrMapManager->GetCurrentRoomIndex()].teleportMakerCount; i++)
	{
		if (teleportMarkers[i].x == currentTile.x && teleportMarkers[i].y == currentTile.y)
		{
			TelePortPlayer();
			m_isInRoom = !m_isInRoom;
			break;
		}
	}

}

void Player::TelePortPlayer()
{
	if (m_isInRoom)
	{
		m_ptrMapManager->SetNewRoom(m_currentRoom);
		m_pos = (m_worldPos - static_cast<float2>(m_ptrMapManager->GetCurrentRoom()) * ROOM_SIZE) * TILE_SIZE;
	}
	else
	{
		m_ptrMapManager->SetNewRoom({ 0,4 });
		m_pos = { ROOM_SIZE_IN_PIXELS / 2, ROOM_SIZE_IN_PIXELS - 100 };
	}
	ChangeCamPos();
}

void Player::Render()
{
	int2 index = { 0,0 };
	animFramesPlayer frame = {};

	if (m_ptrSpritesheet == nullptr) { printf("error:spritesheet isnt initialized properly, in Player::Render\n"); return; }
	
	if (m_playerState & playerIsInAnimation)
	{
		// lerp the rendered pos based on the timer
		float2 renderPos = m_pos;

		// get the curret frame used
		float introCompletion = 1 - (m_introAnimCountDown / INTRO_ANIM_DURATION);
		int2 currentFrame = playerIntroFrameData[static_cast<int>(introCompletion * PLAYER_INTRO_FRAMES)];

		float posX = (ROOM_SIZE_IN_PIXELS / 2 - 100) * (m_introAnimCountDown / INTRO_ANIM_DURATION);
		renderPos.x += posX;

		m_ptrSpritesheet->renderFromScaled(currentFrame, renderPos, PLAYER_SIZE);
		return;
	}
		

	if (m_punchCoolDown > 0)
	{
		switch (m_lookingDir)
		{
			case BaseActor::dirUp:
			{
				frame = frameData[static_cast<int>(animframeName::punchUp)];
				break;
			}
			case BaseActor::dirRight:
			{
				frame = frameData[static_cast<int>(animframeName::punchRight)];
				break;
			}
			case BaseActor::dirDown:
			{
				frame = frameData[static_cast<int>(animframeName::punchDown)];
				break;
			}
			case BaseActor::dirLeft:
			{
				frame = frameData[static_cast<int>(animframeName::punchLeft)];
				break;
			}
			case BaseActor::dirNone:
			default:
			break;
		}

		m_ptrSpritesheet->renderFromScaled(frame.startFrame, m_pos, PLAYER_SIZE);
		return;
	}

	int animFlagMask = (m_playerState & (playerCrouching | playerHasGun | playerIsWalking));


	switch (m_lookingDir)
	{
		case dirUp:
		{
			switch (animFlagMask)
			{
				case playerCrouching:
				case playerCrouching | playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::crouchIdleUp)];
					break;
				}
				case playerCrouching | playerHasGun | playerIsWalking:
				case playerCrouching | playerIsWalking:
				{
					frame = frameData[static_cast<int>(animframeName::crouchingUp)];
					break;
				}
				case playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::standGunUp)];
					break;
				}
				case playerIsWalking | playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::walkGunUp)];
					break;
				}
				case playerIsWalking:
				{
					frame = frameData[static_cast<int>(animframeName::walkUp)];
					break;
				}
				case  0 << 0:
				{
					frame = frameData[static_cast<int>(animframeName::standUp)];
					break;
				}
				default:
				break;
			}
			break;
		}

		case dirRight:
		{
			switch (animFlagMask)
			{
				case playerCrouching:
				case playerCrouching | playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::crouchIdleRight)];
					break;
				}
				case playerCrouching | playerHasGun | playerIsWalking:
				case playerCrouching | playerIsWalking:
				{
					frame = frameData[static_cast<int>(animframeName::crouchingRight)];
					break;
				}
				case playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::standGunRight)];
					break;
				}
				case playerIsWalking | playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::walkGunRight)];
					break;
				}
				case playerIsWalking:
				{
					frame = frameData[static_cast<int>(animframeName::walkRight)];
					break;
				}
				case  0 << 0:
				{
					frame = frameData[static_cast<int>(animframeName::standRight)];
					break;
				}
				default:
				break;
			}
			break;
		}

		case dirDown:
		{
			switch (animFlagMask)
			{
				case playerCrouching:
				case playerCrouching | playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::crouchIdleDown)];
					break;
				}
				case playerCrouching | playerHasGun | playerIsWalking:
				case playerCrouching | playerIsWalking:
				{
					frame = frameData[static_cast<int>(animframeName::crouchingDown)];
					break;
				}
				case playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::standGunDown)];
					break;
				}
				case playerIsWalking | playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::walkGunDown)];
					break;
				}
				case playerIsWalking:
				{
					frame = frameData[static_cast<int>(animframeName::walkDown)];
					break;
				}
				case  0 << 0:
				{
					frame = frameData[static_cast<int>(animframeName::standDown)];
					break;
				}
				default:
				break;
			}
			break;
		}

		case dirLeft:
		{
			switch (animFlagMask)
			{
				case playerCrouching:
				case playerCrouching | playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::crouchIdleLeft)];
					break;
				}
				case playerCrouching | playerIsWalking:
				case playerCrouching | playerHasGun | playerIsWalking:
				{
					frame = frameData[static_cast<int>(animframeName::crouchingLeft)];
					break;
				}
				case playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::standGunLeft)];
					break;
				}
				case playerIsWalking | playerHasGun:
				{
					frame = frameData[static_cast<int>(animframeName::walkGunLeft)];
					break;
				}
				case playerIsWalking:
				{
					frame = frameData[static_cast<int>(animframeName::walkLeft)];
					break;
				}
				case  0 << 0:
				{
					frame = frameData[static_cast<int>(animframeName::standLeft)];
					break;
				}
				default:
				break;
			}
			break;
		}
		// m_lookingDir should never be dirNone
		case dirNone:
		default:
		{
			printf("error:m_lookingDir has a value that inst accepted, in Player::Render\n");
			break;
		}
	}

	if (frame.frameCount < m_animIndex) m_animIndex = 0;

	int2 usedFrame = frame.startFrame;
	if (frame.frameCount != 1)
	{
		usedFrame.x = (frame.startFrame.x + static_cast<int>(m_animIndex));
	}
		
	m_ptrSpritesheet->renderFromScaled(usedFrame, m_pos, PLAYER_SIZE);
	m_lastRenderIndex = usedFrame;

}

const RectInt Player::GetRectFromActor() const
{
	return { static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), PLAYER_SIZE.x, PLAYER_SIZE.y };
}

const Surface* Player::LookAtSurface() const
{
	return m_ptrSpritesheet->getScaledSprite(m_lastRenderIndex, PLAYER_SIZE);
}
