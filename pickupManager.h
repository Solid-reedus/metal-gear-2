#ifndef PICKUP_MANAGER
#define PICKUP_MANAGER

#include "basePickupable.h"
#include "commonTypes.h"

class MapManager;

class PickupManager
{
	public:
	PickupManager(MapManager* p_mapManager, Surface* p_screen);
	//PickupManager();
	void Render();
	bool CheckOverLap(RectInt p_rect);
	void UpdateCurrentPickupAble();
	BasePickupable* CurrentPickable() const { return currentPickAble; };
	BasePickupable* TransferPickable(int p_index);
	void ClearCurrentPickable();

	private:

	BasePickupable* currentPickAble = nullptr;
	static constexpr int PICKUPABLE_TOTAL_SIZE = 3;
	BasePickupable* allPickupable[PICKUPABLE_TOTAL_SIZE] = { nullptr };

	MapManager* m_mapManager = nullptr;
};


#endif // !
