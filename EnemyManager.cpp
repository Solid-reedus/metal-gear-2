#include "precomp.h"
#include "EnemyManager.h"
#include "commonTypes.h"
#include "levelAudioManager.h"
#include "projectile.h"
#include "enemySoundManager.h"

EnemyManager::EnemyManager(char* p_csvEnemYInstructions, const int2* p_camPos, const float2& p_playerPos, Surface* p_ptrScreen, MapManager* p_mapManger, LevelAudioManager* p_levelAudioManager)
	: m_playerPos(p_playerPos)
{
	m_camPos = p_camPos;
	m_ptrSpritesheet = SpriteSheet({ 16,1 }, "assets/enemySprite.png", p_ptrScreen);
	m_levelAudioManager = p_levelAudioManager;

	m_navmesh = &p_mapManger->GetNavMesh();
	m_mapManager = p_mapManger;
	m_ptrArrayEnemies = p_mapManger->GetEnemiesFromCsv(p_csvEnemYInstructions, &m_ptrSpritesheet, m_camPos);

	m_enemySoundManager = EnemySoundManager();

	// init all enemies
	for (int i = 0; i < m_ptrArrayEnemies->size; i++)
	{
		m_ptrArrayEnemies->data[i]->Init(this);
	}

	m_projectilePool = new DymArr<Projectile*>{ new Projectile *[20] , 20 };

	for (int i = 0; i < m_projectilePool->size; i++)
	{
		m_projectilePool->data[i] = new Projectile({ 8,8 }, 0xffffff, p_ptrScreen);
	}


}

void EnemyManager::Update(float p_deltaTime)
{
	for (size_t i = 0; i < m_ptrArrayEnemies->size; i++)
	{
		m_ptrArrayEnemies->data[i]->Update(p_deltaTime);
		m_ptrArrayEnemies->data[i]->Render();
	}

	if (m_timer > 0)
	{
		m_timer -= p_deltaTime;
		if (m_gameState == GameplayState::alert)
		{
			StayAlert();
		}
	}
	if (m_gameState != GameplayState::normal && m_timer < 1)
	{
		switch (m_gameState)
		{
		case EnemyManager::GameplayState::alert:
		{
			m_gameState = GameplayState::evasion;
			m_levelAudioManager->ChangeTrack(LevelAudioManager::CurrentSong::evasion);
			GoIntoEvasion();
			m_timer = 9999;
			break;
		}
		case EnemyManager::GameplayState::evasion:
		{
			m_gameState = GameplayState::caution;
			m_levelAudioManager->ChangeTrack(LevelAudioManager::CurrentSong::caution);
			GoBackToPatrol();
			m_timer = 9999;
			break;
		}
		case EnemyManager::GameplayState::caution:
		{
			m_levelAudioManager->ChangeTrack(LevelAudioManager::CurrentSong::outside);
			m_gameState = GameplayState::normal;
			break;

		}
		default:
		break;
		}
	}

}

NavNode* EnemyManager::GetClosestNavNode(int2 p_pos) const
{
	NavNode* lowestNavNode = nullptr;

	for (size_t i = 1; i < m_navmesh->size; i++)
	{
		if (lowestNavNode == nullptr || manhattanDisInt(p_pos, m_navmesh->navNodes[i].pos) < manhattanDisInt(p_pos, lowestNavNode->pos))
		{

			if (abs(p_pos.x - m_navmesh->navNodes[i].pos.x) < 1 || p_pos.y - m_navmesh->navNodes[i].pos.y < 1)
			{
				lowestNavNode = &m_navmesh->navNodes[i];
			}
		}
	}

	return lowestNavNode;
}

NavNode* EnemyManager::GetClosestNavNodeWithCol(int2 p_pos, int roomIndex) const
{
	NavNode* lowestNavNode = nullptr;
	int2* colliders = m_mapManager->rooms->data[roomIndex].ptrArrayColliders;

	for (int i = 1; i < m_navmesh->size; i++)
	{
		if (!(abs(p_pos.x - m_navmesh->navNodes[i].pos.x) < 2 || p_pos.y - m_navmesh->navNodes[i].pos.y < 2))
		{
			continue;
		}

		if (lowestNavNode == nullptr || manhattanDisInt(p_pos, m_navmesh->navNodes[i].pos) < manhattanDisInt(p_pos, lowestNavNode->pos))
		{
			bool colIsInWay = false;
			int2 posA = p_pos;
			int2 posB = m_navmesh->navNodes[i].pos;

			// Bresenham's line algorithm for line traversal between posA and posB
			int dx = abs(posB.x - posA.x);
			int dy = abs(posB.y - posA.y);
			int sx = (posA.x < posB.x) ? 1 : -1;
			int sy = (posA.y < posB.y) ? 1 : -1;
			int err = dx - dy;

			int x = posA.x;
			int y = posA.y;

			while (!colIsInWay)
			{
				// Collision check with colliders
				for (int j = 0; j < m_mapManager->rooms->data[roomIndex].colliderCount; j++)
				{
					if (colliders[j].x == x % ROOM_SIZE && colliders[j].y == y % ROOM_SIZE)
					{
						colIsInWay = true; // Mark as collision
						break;
					}
				}

				// If we've reached the end point, stop
				if (x == posB.x && y == posB.y) break;

				// Move along the line
				int e2 = 2 * err;
				if (e2 > -dy) { err -= dy; x += sx; }
				if (e2 < dx) { err += dx; y += sy; }
			}

			if (!colIsInWay)
			{
				lowestNavNode = &m_navmesh->navNodes[i];
			}
		}
	}

	if (lowestNavNode == nullptr)
	{
		printf("something went wrong\n");
	}
	return lowestNavNode;
}


DymArr<NavNode*>* EnemyManager::GetPath(NavNode* p_startNode, NavNode* p_endNode)
{
	if (p_startNode == nullptr || p_endNode == nullptr) { return {}; }

	const int MAX_EXPLOREABLE_NODES = 80;
	NavNodeQueue YetToExploreQueue(MAX_EXPLOREABLE_NODES);
	NavNode** exploredNodes = new NavNode * [MAX_EXPLOREABLE_NODES];
	int exploredNodesSize = 0;
	bool foundPath = false;
	NavNode* currentNode = nullptr;
	NavNode* closestNode = nullptr;

	// Explore nodes based on A*
	while (!foundPath)
	{
		if (YetToExploreQueue.Getsize() < 1 && currentNode != nullptr)
		{
			// Only delete exploredNodes once at the end if returning
			delete[] exploredNodes;
			return GetPath(p_startNode, closestNode);
		}

		if (exploredNodesSize < 1)
		{
			currentNode = p_startNode;
			exploredNodes[exploredNodesSize++] = currentNode;

			if (exploredNodesSize >= MAX_EXPLOREABLE_NODES)
			{
				delete[] exploredNodes;
				return nullptr;
			}
		}
		else
		{
			currentNode = YetToExploreQueue.GetFirst();

			bool firstIsValid = true;
			for (int j = 0; j < exploredNodesSize; j++)
			{
				if (exploredNodes[j]->pos == currentNode->pos)
				{
					firstIsValid = false;
					break;
				}
			}

			NavNode** queue = YetToExploreQueue.LookAtQueue();
			for (uint i = YetToExploreQueue.GetStartPitch(); i < YetToExploreQueue.GetEndPitch(); i++)
			{
				NavNode* checkedNode = queue[i];
				if (GetStepCost(currentNode, p_startNode, p_endNode) > GetStepCost(checkedNode, p_startNode, p_endNode) || !firstIsValid)
				{
					bool isAlreadyExplored = false;
					for (int j = 0; j < exploredNodesSize; j++)
					{
						if (exploredNodes[j]->pos == checkedNode->pos)
						{
							isAlreadyExplored = true;
							break;
						}
					}
					if (!isAlreadyExplored)
					{
						currentNode = checkedNode;
					}
				}
			}

			exploredNodes[exploredNodesSize++] = currentNode;
			if (exploredNodesSize >= MAX_EXPLOREABLE_NODES)
			{
				delete[] exploredNodes;
				return nullptr;
			}
		}

		if (closestNode == nullptr || GetStepCost(closestNode, p_startNode, p_endNode) > GetStepCost(currentNode, p_startNode, p_endNode))
		{
			closestNode = currentNode;
		}

		NavNode* adjacentNodes[4] = { currentNode->t, currentNode->r, currentNode->b, currentNode->l };
		for (int i = 0; i < 4; i++)
		{
			if (adjacentNodes[i] == p_endNode)
			{
				foundPath = true;
				break;
			}
			else if (adjacentNodes[i] != nullptr)
			{
				bool isAlreadyExplored = false;
				for (int j = 0; j < exploredNodesSize; j++)
				{
					if (exploredNodes[j]->pos == adjacentNodes[i]->pos)
					{
						isAlreadyExplored = true;
						break;
					}
				}
				if (!isAlreadyExplored)
				{
					YetToExploreQueue.AddAtEnd(adjacentNodes[i]);
				}
			}
		}
	}

	// Create the path array and cleanup
	int pathCount = exploredNodesSize;
	NavNode** pathArray = new NavNode * [pathCount];
	for (int i = 0; i < pathCount; i++)
	{
		pathArray[i] = exploredNodes[i];
	}

	// Delete exploredNodes only once before exiting the function
	delete[] exploredNodes;

	// Return the path
	return new DymArr<NavNode*>{ pathArray, pathCount };
}





void EnemyManager::GoIntoAlert()
{

	m_timer = 9999;

	if (m_gameState != GameplayState::alert)
	{
		m_gameState = GameplayState::alert;
		m_levelAudioManager->ChangeTrack(LevelAudioManager::CurrentSong::alert, 1500);
		m_enemySoundManager.Play(EnemySoundManager::alert);
	}

	
	for (int i = 0; i < m_ptrArrayEnemies->size; i++)
	{
		Enemy* e = m_ptrArrayEnemies->data[i];
		if (e->IsAlive())
		{
			e->GoIntoAlert();
		}
		else 
		{
			int2 pos = e->GetStartPos() * TILE_SIZE;
			// if the enemy isnt in the same room as player then respawn in alert phase
			if (!(pos.x > m_camPos->x && pos.x< m_camPos->x + ROOM_SIZE_IN_PIXELS &&
				pos.y> m_camPos->y && pos.y < m_camPos->y + ROOM_SIZE_IN_PIXELS))
			{
				e->Respawn();
				e->GoIntoAlert();
			}
		}
	}
	

}

void EnemyManager::StayAlert()
{
	for (int i = 0; i < m_ptrArrayEnemies->size; i++)
	{
		Enemy* e = m_ptrArrayEnemies->data[i];
		if (e->IsAlive())
		{
			e->StayInAlert();
		}
	}
}

void EnemyManager::GoIntoEvasion()
{
	for (int i = 0; i < m_ptrArrayEnemies->size; i++)
	{
		Enemy* e = m_ptrArrayEnemies->data[i];
		if (e->IsAlive())
		{
			e->GoIntoEvasion();
		}
	}
}

void EnemyManager::GoBackToPatrol()
{
	for (int i = 0; i < m_ptrArrayEnemies->size; i++)
	{
		Enemy* e = m_ptrArrayEnemies->data[i];
		if (e->IsAlive())
		{
			e->GoBackToPatrol();
		}
	}
}

void EnemyManager::Shoot(float2 p_pos, float2 p_dir)
{
	Projectile* usedProj = m_projectilePool->data[projectileIndex];
	usedProj->Shoot(p_pos, p_dir);
	projectileIndex++;
	if (projectileIndex >= m_projectilePool->size) { projectileIndex = 0; }
}

int EnemyManager::GetStepCost(NavNode* p_currentNode, NavNode* p_startNode, NavNode* p_endNode)
{
	return static_cast<int>(manhattanDisInt(p_currentNode->pos, p_startNode->pos) / 2) + manhattanDisInt(p_currentNode->pos, p_endNode->pos);
}


