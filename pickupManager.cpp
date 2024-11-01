#include "precomp.h"
#include "pickupManager.h"
#include "commonTypes.h"
#include "AllEquippableHeader.h"
#include "basePickupable.h"
#include "MapManager.h"
#include "renderingConst.h"

//PickupManager::PickupManager()
PickupManager::PickupManager(MapManager* p_mapManager, Surface* p_screen)
{
	// have predefined items and each room marked with it
	// make a update that checks if the player collides with the pickup
	// have flags from spawning
	// add a funtion to the player to add the item to the player

	m_mapManager = p_mapManager;

	SpriteSheet* pickupSpritesheet = new SpriteSheet({ 3,3 }, "assets/items.png", p_screen);

	RectInt pistolRect = { ROOM_SIZE_IN_PIXELS / 2 , ROOM_SIZE_IN_PIXELS / 2 , 100, 60 };
	
	allPickupable[0] = new Pistol({ 2, 1 }, pickupSpritesheet, pistolRect);
	

}

void PickupManager::Render()
{
	if (currentPickAble != nullptr)
	{
		currentPickAble->Render();
	}
}

bool PickupManager::CheckOverLap(RectInt p_rect)
{
	if (currentPickAble == nullptr)
	{
		return false;
	}

	return currentPickAble->GetRect().Overlaps(p_rect);
}

void PickupManager::UpdateCurrentPickupAble()
{
	int2 currentRoom = m_mapManager->GetCurrentRoom();
	currentPickAble = nullptr;

	for (int i = 0; i < PICKUPABLE_TOTAL_SIZE; i++)
	{
		if (allPickupable[i] == nullptr)
		{
			continue;
		}
		if (allPickupable[i]->GetRoomIndex() == currentRoom)
		{
			currentPickAble = allPickupable[i];
			break;
		}
	}

}

BasePickupable* PickupManager::TransferPickable(int p_index)
{
	if (p_index >= PICKUPABLE_TOTAL_SIZE)
	{
		return nullptr;
	}
	BasePickupable* returnedItem = allPickupable[p_index];
	allPickupable[p_index] = nullptr;
	return returnedItem;
}

void PickupManager::ClearCurrentPickable()
{
	int flag = BasePickupable::pickUpflag::pickupRespawn;
	if ((flag & currentPickAble->GetFlags()) != flag)
	{
		for (BasePickupable*& index : allPickupable)
		{
			if (index == currentPickAble)
			{
				index = nullptr;
			}
		}
	}
	currentPickAble = nullptr;
}

