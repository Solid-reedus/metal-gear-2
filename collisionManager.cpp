#include "precomp.h"
#include "collisionManager.h"
#include "Player.h"
#include "projectile.h"
#include "MapManager.h"
#include "pickupManager.h"
#include "basePickupable.h"
#include "baseEquippable.h"
#include "BaseActor.h"
#include "renderingConst.h"
#include "commonTypes.h"

CollisionManager::CollisionManager(EnemyManager* p_enemyManager, Player* p_player, MapManager* p_mapManager, Surface* p_screen)
	//: m_pickupManager(p_mapManager->GetPickupManager())
{

	m_enemyManager = p_enemyManager;
	m_player = p_player;
	playerProjectilePool = m_player->GetProjectilesPool();
	enemyProjectilePool = m_enemyManager->GetProjectilesPool();
	m_mapManager = p_mapManager;
	m_pickupManager = p_mapManager->GetPickupManager();
	m_screen = p_screen;

	m_mapIndex = m_mapManager->GetCurrentRoomIndex();


	DymArr<Enemy*>* tempEnemyArr = m_enemyManager->m_ptrArrayEnemies;
	int size = tempEnemyArr->size;
	enemyDymArr = new DymArr<Enemy*>{ new Enemy *[size], size };
	for (int i = 0; i < size; i++)
	{
		enemyDymArr->data[i] = tempEnemyArr->data[i];
	}
}

bool CollisionManager::IsPointInFOV(const float2& p_point, float2 p_startPos, float p_angleToA, float p_angleToB)
{
	// Calculate the angle to the point from the enemy's position
	float angleToPoint = atan2(p_point.y - p_startPos.y, p_point.x - p_startPos.x);

	// Normalize angles to the range [0, 2*PI]
	while (angleToPoint < 0) angleToPoint += 2 * PI;
	while (p_angleToA < 0) p_angleToA += 2 * PI;
	while (p_angleToB < 0) p_angleToB += 2 * PI;

	// Check if the point's angle is within the FOV
	if (p_angleToA < p_angleToB)
	{
		return (angleToPoint >= p_angleToA && angleToPoint <= p_angleToB);
	}
	else
	{
		// Handle wrap-around
		return (angleToPoint >= p_angleToA || angleToPoint <= p_angleToB);
	}
}

bool CollisionManager::CheckIfPlayerIsSpotted()
{
	float2 camPos = { static_cast<float>(m_player->GetCamPos()->x), static_cast<float>(m_player->GetCamPos()->y) };

	for (int i = 0; i < enemyDymArr->size; i++)
	{
		Enemy* enemy = enemyDymArr->data[i];

		if (!enemy->IsAlive() || enemy->IsStunned()) continue;

		float2 pos = enemyDymArr->data[i]->GetPos() - camPos;
		if (pos.x < MAP_RENDERING_START || pos.x > ROOM_SIZE_IN_PIXELS ||
			pos.y < MAP_RENDERING_START || pos.y > ROOM_SIZE_IN_PIXELS)
		{
			continue;
		}

		int2 enemyPos = { static_cast<int>(enemy->GetPos().x - camPos.x), static_cast<int>(enemy->GetPos().y - camPos.y) };
		const float2 lookdir = enemy->GetLookDir();

		float fovAngle = 20.0f;

		float maxRange = ROOM_SIZE_IN_PIXELS; // Max distance the enemy can see

		float halfFov = fovAngle / 2.0f;

		float2 startPos = { static_cast<float>(enemyPos.x + (enemy->ENEMY_RENDER_SIZE.x / 2)),static_cast<float>(enemyPos.y + (enemy->ENEMY_RENDER_SIZE.y / 5)) };
		float2 endPosA = { 0,0 };
		float2 endPosB = { 0,0 };

		float angleToA = atan2(lookdir.y, lookdir.x) - (halfFov * (PI / 180.0f));
		float angleToB = atan2(lookdir.y, lookdir.x) + (halfFov * (PI / 180.0f));

		endPosA.x = startPos.x + cos(angleToA) * maxRange;
		endPosA.y = startPos.y + sin(angleToA) * maxRange;

		endPosB.x = startPos.x + cos(angleToB) * maxRange;
		endPosB.y = startPos.y + sin(angleToB) * maxRange;

		if (isnan_custom(startPos.x) || isnan_custom(startPos.y) ||
			isnan_custom(endPosA.x) || isnan_custom(endPosA.y) ||
			isnan_custom(endPosB.x) || isnan_custom(endPosB.y))
		{
			continue; // Skip this iteration if any position is invalid
		}

		#ifdef _DEBUG
		m_screen->Line(startPos.x, startPos.y, endPosA.x, endPosA.y, 0xf0ff0f);
		m_screen->Line(startPos.x, startPos.y, endPosB.x, endPosB.y, 0xf0ff0f);
		#endif

		float minX = (startPos.x < endPosA.x) ? ((startPos.x < endPosB.x) ? startPos.x : endPosB.x) : ((endPosA.x < endPosB.x) ? endPosA.x : endPosB.x);
		float maxX = (startPos.x > endPosA.x) ? ((startPos.x > endPosB.x) ? startPos.x : endPosB.x) : ((endPosA.x > endPosB.x) ? endPosA.x : endPosB.x);
		float minY = (startPos.y < endPosA.y) ? ((startPos.y < endPosB.y) ? startPos.y : endPosB.y) : ((endPosA.y < endPosB.y) ? endPosA.y : endPosB.y);
		float maxY = (startPos.y > endPosA.y) ? ((startPos.y > endPosB.y) ? startPos.y : endPosB.y) : ((endPosA.y > endPosB.y) ? endPosA.y : endPosB.y);


		RectInt lookRect =
		{
			static_cast<int>(minX),            // x position
			static_cast<int>(minY),            // y position
			static_cast<int>(maxX - minX),     // width
			static_cast<int>(maxY - minY)      // height
		};

		#ifdef _DEBUG
		m_screen->Box(lookRect.x, lookRect.y, lookRect.x + lookRect.w, lookRect.y + lookRect.h, 0xf0ff0f);
		#endif

		RectInt playerRect = m_player->GetRectFromActor();
		playerRect.w /= 2;
		playerRect.x += (playerRect.w / 2);

		#ifdef _DEBUG
		m_screen->Box(playerRect.x, playerRect.y, playerRect.x + playerRect.w, playerRect.y + playerRect.h, 0x1111ff);
		#endif

		Surface* collisionMask = m_mapManager->rooms->data[m_mapIndex].collisionMask;


		if (lookRect.Overlaps(playerRect))
		{
			int2 pointsToCheck[5] =
			{
				{playerRect.x, playerRect.y},										    // top left
				{playerRect.x + playerRect.w, playerRect.y},						    // top right
				{playerRect.x + playerRect.w, playerRect.y + playerRect.h},			    // bottom right
				{playerRect.x, playerRect.y + playerRect.h},						    // bottom left
				{playerRect.x + (playerRect.w / 2), playerRect.y + (playerRect.h / 2)}, // center
			};

			for (int2& point : pointsToCheck)
			{
				if (IsPointInFOV(point, startPos, angleToA, angleToB))
				{
					if (collisionMask->RayCastHit({ static_cast<int>(startPos.x), static_cast<int>(startPos.y) }, { playerRect.x, playerRect.y }))
					{
						printf("mask hit");
						return true;
					}
				}
			}

		}
	}

	return false;
}


void CollisionManager::Update(float p_deltaTime)
{
	if (m_mapIndex != m_mapManager->GetCurrentRoomIndex())
	{
		m_mapIndex = m_mapManager->GetCurrentRoomIndex();

		for (int i = 0; i < playerProjectilePool->size; i++)
		{
			playerProjectilePool->data[i]->m_active = false;
		}

		for (int i = 0; i < enemyProjectilePool->size; i++)
		{
			enemyProjectilePool->data[i]->m_active = false;
		}

		return;
	}

	//update each item that should check for col
	for (int i = 0; i < playerProjectilePool->size; i++)
	{
		Projectile*& proj = playerProjectilePool->data[i];
		if (proj->m_active)
		{
			proj->Update(p_deltaTime);

			for (int j = 0; j < enemyDymArr->size; j++)
			{
				if (!enemyDymArr->data[j]->IsAlive()) continue;

				if (OverLapsPixelPerfect(proj, enemyDymArr->data[j]))
				{
					proj->m_active = false;
					enemyDymArr->data[j]->TakeDamage(proj->m_damage);

				}
			}
		}
	}

	for (int i = 0; i < enemyProjectilePool->size; i++)
	{
		Projectile*& proj = enemyProjectilePool->data[i];
		if (proj->m_active)
		{
			proj->Update(p_deltaTime);

			if (OverLapsPixelPerfect(proj, m_player))
			{
				proj->m_active = false;
				m_player->TakeDamage(proj->m_damage);

			}
		}
	}


	if (!m_player->CheckIfPlayerIsPunching()) {playerHasPunched = false;}

	if (!playerHasPunched && m_player->CheckIfPlayerIsPunching())
	{
		
		float2 pos = m_player->GetPLayerWorldPos();
		float2 playerDir = m_player->LookAtPlayerWalkDir();
		RectInt punchHurtBox = m_player->GetRectFromActor();

		punchHurtBox.w /= 2;
		punchHurtBox.h /= 2;
		punchHurtBox.x += ((punchHurtBox.w / 2) + static_cast<int>(playerDir.x * 30));
		punchHurtBox.y += ((punchHurtBox.h / 2) + static_cast<int>(playerDir.y * 40));

		for (int i = 0; i < enemyDymArr->size; i++)
		{
			RectInt enemyHitBox = enemyDymArr->data[i]->GetRectFromActor();

			if (punchHurtBox.Overlaps(enemyHitBox))
			{
				enemyDymArr->data[i]->TakeDamage(20);
				playerHasPunched = true;
			}

			#ifdef _DEBUG
			m_screen->Box(enemyHitBox.x, enemyHitBox.y, enemyHitBox.x + enemyHitBox.w, enemyHitBox.y + enemyHitBox.h, 0x11717f);
			#endif

		}

		#ifdef _DEBUG
		m_screen->Box(punchHurtBox.x, punchHurtBox.y, punchHurtBox.x + punchHurtBox.w, punchHurtBox.y + punchHurtBox.h, 0x00f0f0);
		#endif

	}


	BasePickupable* currentPickable = m_pickupManager->CurrentPickable();
	
	if (currentPickable != nullptr)
	{
		int flag = BasePickupable::pickUpflag::pickupInTruck;
		if (((flag & currentPickable->GetFlags()) == flag) && m_player->PlayerIsInRoom())
		{
			if (m_pickupManager->CheckOverLap(m_player->GetRectFromPlayer()))
			{
				baseWeapon* weapon = dynamic_cast<baseWeapon*>(currentPickable);
				if (weapon != nullptr)
				{
					m_player->AddWeapon(weapon);
					m_pickupManager->ClearCurrentPickable();

				}
			}
		}
	}

	if (CheckIfPlayerIsSpotted())
	{
		m_enemyManager->GoIntoAlert();
	}
}


void CollisionManager::Render()
{
	for (int i = 0; i < playerProjectilePool->size; i++)
	{
		Projectile* proj = playerProjectilePool->data[i];
		if (proj->m_active)
		{
			proj->Render();

		}
	}

	for (int i = 0; i < enemyProjectilePool->size; i++)
	{
		Projectile* proj = enemyProjectilePool->data[i];
		if (proj->m_active)
		{
			proj->Render();
		}
	}


	const BasePickupable* currentPickable = m_pickupManager->CurrentPickable();
	if (currentPickable != nullptr)
	{
		int flag = BasePickupable::pickUpflag::pickupInTruck;
		if (((flag & currentPickable->GetFlags()) == flag) && m_player->PlayerIsInRoom())
		{
			m_pickupManager->Render();
		}
	}

}

// this code is based on croakingkero's code 
// https://croakingkero.com/tutorials/pixel_collision_detection/
// and also inspired by Hesam [PR Y1] 
bool CollisionManager::OverLapsPixelPerfect(const Projectile* p_projectile,const BaseActor* p_actor)
{
	RectInt projRect = p_projectile->GetRectBasedOnSurface();
	RectInt actorRect = p_actor->GetRectFromActor();

	if (actorRect.x < MAP_RENDERING_START || actorRect.x > ROOM_SIZE_IN_PIXELS ||
		actorRect.y < MAP_RENDERING_START || actorRect.y > ROOM_SIZE_IN_PIXELS)
	{
		return false;
	}


	// early return
	if (!projRect.Overlaps(actorRect)) {return false;}

	// boundary top
	int overlapT = (projRect.y > actorRect.y) ? projRect.y : actorRect.y;
	// boundary right
	int overlapR = (projRect.x + projRect.w < actorRect.x + actorRect.w) ? projRect.x + projRect.w : actorRect.x + actorRect.w;         
	// boundary bottom
	int overlapB = (projRect.y + projRect.h < actorRect.y + actorRect.h) ? projRect.y + projRect.h : actorRect.y + actorRect.h;         
	// boundary left
	int overlapL = (projRect.x > actorRect.x) ? projRect.x : actorRect.x;

	// Loop through the overlapping area
	for (int i = overlapT; i < overlapB; ++i)
	{
		for (int j = overlapL; j < overlapR; ++j)
		{
			// Relative position in the actor's surface
			int relActorPosX = j - actorRect.x;
			int relActorPosY = i - actorRect.y;

			uint pixelActor = p_actor->LookAtSurface()->pixels[relActorPosY * p_actor->LookAtSurface()->width + relActorPosX];

			if ((pixelActor & TRANSPARENCY_MASK) != TRANSPARENCY_MASK)
			{
				printf("overlaps pixel perf \n");
				return true;
			}
		}
	}

	// No pixel-perfect overlap detected
	return false;
}

