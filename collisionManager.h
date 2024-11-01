#ifndef COLLISION_MANAGER
#define COLLISION_MANAGER

#include "Player.h"
#include "projectile.h"
#include "BaseActor.h"

class EnemyManager;
class MapManager;
class PickupManager;

// the implementation of collision's is based on Tom's [PR Y1] structure
class CollisionManager
{
	public:
	CollisionManager(EnemyManager* p_enemyManager, Player* p_player, MapManager* p_mapManager, Surface* p_screen);

	void Update(float p_deltaTime);
	void Render();

	PickupManager* GetPickUpManager() const { return m_pickupManager; };

	// this code is based on croakingkero's code 
	// https://croakingkero.com/tutorials/pixel_collision_detection/
	// and also inspired by Hesam [PR Y1] 
	bool OverLapsPixelPerfect(const Projectile* p_projectile, const BaseActor* p_actor);

	// this funtion was made based on jorisvanleeuwengames https://jorisvanleeuwengames.com/content/projects/other/tutorials/trigonometry.pdf
	// and help of chatgpt
	bool IsPointInFOV(const float2& p_point, float2 p_startPos, float p_angleToA, float p_angleToB);

	bool CheckIfPlayerIsSpotted();

	// this funtion is based on chatgpt, it will check if the value is valid
	bool isnan_custom(float value) {return value != value;}

	private:
	EnemyManager* m_enemyManager; 
	Player* m_player;

	// mapindex is used for checking if the rooms changed
	int m_mapIndex;

	bool playerHasPunched = false;

	const DymArr<Enemy*>* enemyDymArr;
	const DymArr<Projectile*>* playerProjectilePool;
	const DymArr<Projectile*>* enemyProjectilePool;
	const MapManager* m_mapManager;
	PickupManager* m_pickupManager;
	Surface* m_screen;

};


#endif // !COLLISION_MANAGER

