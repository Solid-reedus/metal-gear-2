#include "precomp.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "commonTypes.h"


Enemy::Enemy(SpriteSheet* p_ptrSpritesheet, const int2* p_camPos, InstructionsStruct* p_ptrArrayPatrolRoute, int2 p_startPos, int2& p_mapRoomSize)
	: m_mapRoomSize(p_mapRoomSize)
{
	m_ptrSpritesheet = p_ptrSpritesheet;
	m_ptrEnemyManger = nullptr;
	m_alertPath = nullptr;

	m_lookingDir = dirNone;
	m_walkDir = dirNone;
	m_camPos = p_camPos;

	m_startPos = p_startPos;
	m_pos = static_cast<float2>(m_startPos * TILE_SIZE);

	m_ptrArrayPatrolRoute = p_ptrArrayPatrolRoute;
	m_patrolSize = m_ptrArrayPatrolRoute->instructionCount;

	m_lastNode = nullptr;
	m_NextNode = nullptr;
}

const RectInt Enemy::GetRectFromActor() const
{
	float2 screenSpace = m_pos - *m_camPos;
	return { static_cast<int>(screenSpace.x), static_cast<int>(screenSpace.y), ENEMY_RENDER_SIZE.x, ENEMY_RENDER_SIZE.y };
}

void Enemy::TakeDamage(int p_amount)
{
	m_health -= p_amount;
	m_stunTimer = STUN_DUR - 1;
	if (m_health < 1)
	{
		m_isAlive = false;
	}
}

void Enemy::Respawn()
{
	m_isAlive = true;
	m_pos = static_cast<float2>(m_startPos * TILE_SIZE);
	m_lastNode = nullptr;
	m_NextNode = nullptr;
	m_health = 60;
	m_stunTimer = STUN_DUR;
}

Surface* Enemy::LookAtSurface() const
{
	return m_ptrSpritesheet->getScaledSprite({ m_lastUsedFrame , 0 }, ENEMY_RENDER_SIZE);
}

NavNode* Enemy::GetNextNodeNavNode()
{
	if (m_lastNode == nullptr) { printf("warning:m_lastNode is nullptr so getting next node is not possible, in Enemy::GetNextNodeNavNode\n"); return nullptr; }

	switch (m_ptrArrayPatrolRoute->instructions[m_patrolIndex].enemyInstruction)
	{
		case Enemy::EnemyInstructions::waitUp:
		case Enemy::EnemyInstructions::walkUp:
		{
			if (m_lastNode->t != nullptr) return m_lastNode->t;
			break;
		}
		case Enemy::EnemyInstructions::waitRight:
		case Enemy::EnemyInstructions::walkRight:
		{
			if (m_lastNode->r != nullptr) return m_lastNode->r;
			break;
		}
		case Enemy::EnemyInstructions::waitDown:
		case Enemy::EnemyInstructions::walkDown:
		{
			if (m_lastNode->b != nullptr) return m_lastNode->b;
			break;
		}
		case Enemy::EnemyInstructions::waitLeft:
		case Enemy::EnemyInstructions::walkLeft:
		{
			if (m_lastNode->l != nullptr) return m_lastNode->l;
			break;
		}
		case Enemy::EnemyInstructions::none:
		default:
		break;
	}

	printf("warning:GetNextNodeNavNode is returning a nullptr because m_ptrArrayPatrolRoute->instructions[m_patrolIndex].enemyInstruction was invalid value, in Enemy::GetNextNodeNavNode\n");
	return nullptr;
}

void Enemy::Init(EnemyManager* p_ptrEnemyManger)
{
	m_ptrEnemyManger = p_ptrEnemyManger;
	m_lastNode = m_ptrEnemyManger->GetClosestNavNode(m_startPos);
	m_NextNode = GetNextNodeNavNode();
	UpdateInstructions();

}

void Enemy::GoIntoAlert()
{
	m_enemyState = EnemyState::alert;

	//currentroom.x + currentroom.y * 3
	
	int2 pos = { static_cast<int>(m_camPos->x) / ROOM_SIZE_IN_PIXELS, static_cast<int>(m_camPos->y) / ROOM_SIZE_IN_PIXELS };
	int enemyRoomIndex = pos.x + pos.y * m_mapRoomSize.x;
	int playerRoomIndex = (m_camPos->x / ROOM_SIZE_IN_PIXELS) + (m_camPos->y / ROOM_SIZE_IN_PIXELS) * m_mapRoomSize.x;
	NavNode* closestNode = m_ptrEnemyManger->GetClosestNavNodeWithCol({ static_cast<int>(m_pos.x / TILE_SIZE), static_cast<int>(m_pos.y / TILE_SIZE) }, enemyRoomIndex);;
	const float2& playerPosfloat = m_ptrEnemyManger->GetPlayerPos();
	NavNode* playerPos = m_ptrEnemyManger->GetClosestNavNodeWithCol({ static_cast<int>(playerPosfloat.x), static_cast<int>(playerPosfloat.y) }, playerRoomIndex);
	// something with the pathindex might go wrong


	if (m_alertPath != nullptr) {delete m_alertPath;}

	// !!! needs rework if the enemies are far away from a node they might spaz out
	m_alertPath = m_ptrEnemyManger->GetPath(closestNode, playerPos);
	m_lastSeenPlayer = normalize(m_ptrEnemyManger->GetPlayerPos() - (m_pos / TILE_SIZE));


}

void Enemy::StayInAlert()
{
	if (m_alertPath == nullptr)
	{
		return;
	}
	if (m_alertPathIndex >= m_alertPath->size)
	{
		
		int enemyRoomIndex = (static_cast<int>(m_pos.x) / ROOM_SIZE_IN_PIXELS) + (static_cast<int>(m_pos.y) / ROOM_SIZE_IN_PIXELS) * m_mapRoomSize.x;
		int playerRoomIndex = (m_camPos->x / ROOM_SIZE_IN_PIXELS) + (m_camPos->y / ROOM_SIZE_IN_PIXELS) * m_mapRoomSize.x;

		NavNode* closestNode = m_ptrEnemyManger->GetClosestNavNodeWithCol({ static_cast<int>(m_pos.x / TILE_SIZE), static_cast<int>(m_pos.y / TILE_SIZE) }, enemyRoomIndex);;
		const float2& playerPosfloat = m_ptrEnemyManger->GetPlayerPos();
		NavNode* playerPos = m_ptrEnemyManger->GetClosestNavNodeWithCol({ static_cast<int>(playerPosfloat.x), static_cast<int>(playerPosfloat.y)}, playerRoomIndex);
		// something with the pathindex might go wrong
	
		if (m_alertPath != nullptr) { delete m_alertPath; }
	
		// !!! needs rework if the enemies are far away from a node they might spaz out
		m_alertPath = m_ptrEnemyManger->GetPath(closestNode, playerPos);
	
		m_alertPathIndex = 0;
	}
	m_lastSeenPlayer = normalize(m_ptrEnemyManger->GetPlayerPos() - (m_pos / TILE_SIZE));

}

void Enemy::GoIntoEvasion()
{
	m_enemyState = EnemyState::evasion;
	m_lastSeenPlayer = normalize(m_ptrEnemyManger->GetPlayerPos() - (m_pos / TILE_SIZE));


}

void Enemy::GoBackToPatrol()
{
	m_enemyState = EnemyState::goingBackToPatrol;

	int2 pos = { static_cast<int>(m_camPos->x) / ROOM_SIZE_IN_PIXELS, static_cast<int>(m_camPos->y) / ROOM_SIZE_IN_PIXELS };
	int enemyRoomIndex = pos.x + pos.y * m_mapRoomSize.x;

	NavNode* closestNode = m_ptrEnemyManger->GetClosestNavNodeWithCol({ static_cast<int>(m_pos.x / TILE_SIZE), static_cast<int>(m_pos.y / TILE_SIZE) }, enemyRoomIndex);;

	if (m_alertPath != nullptr) { delete m_alertPath; }

	m_alertPath = m_ptrEnemyManger->GetPath(closestNode, m_NextNode);
	m_alertPathIndex = 0;

}

void Enemy::Update(float p_deltaTime)
{
	if (!m_isAlive) { return; };

	m_animIndex += p_deltaTime / 333;

	m_dir = { 0,0 };

	if (m_stunTimer != STUN_DUR)
	{
		m_stunTimer -= p_deltaTime;
		if (m_stunTimer < 0)
		{
			m_ptrEnemyManger->GoIntoAlert();
			m_stunTimer = STUN_DUR;

		}
		return;
	}



	switch (m_enemyState)
	{
		case Enemy::EnemyState::alert:
		{

			int2 pos = { static_cast<int>(m_pos.x) / TILE_SIZE, static_cast<int>(m_pos.y) / TILE_SIZE };
			int2 playerPos = { static_cast<int>(m_ptrEnemyManger->GetPlayerPos().x), static_cast<int>(m_ptrEnemyManger->GetPlayerPos().y) };
			if (manhattanDisInt(pos, playerPos) < 15 )
			{
				if (m_shootTimer > 1)
				{
					m_shootTimer -= p_deltaTime;
					if (m_shootTimer < 1)
					{
						float2 scrreenPos = { static_cast<float>(static_cast<int>(m_pos.x) % ROOM_SIZE_IN_PIXELS), static_cast<float>(static_cast<int>(m_pos.y) % ROOM_SIZE_IN_PIXELS) };
						float2 playerDir = normalize(playerPos - pos);
						float2 bulletStart = { scrreenPos.x + (ENEMY_RENDER_SIZE.x / 2 + (playerDir.x * 30)), scrreenPos.y + (ENEMY_RENDER_SIZE.y / 3 + (playerDir.y * 30)) };

						m_ptrEnemyManger->Shoot(bulletStart, playerDir);
						m_shootTimer = SHOOT_COOL_DOWN;
					}
				}
				break;
			}

			if (m_alertPath == nullptr)
			{
				break;
			}
			if (m_alertPathIndex >= m_alertPath->size)
			{
				break;
			}


			NavNode* currentNode = m_alertPath->data[m_alertPathIndex];
			m_dir = static_cast<float2>(currentNode->pos * TILE_SIZE) - m_pos;
			m_dir = normalize(m_dir);


			// if the enemy isnt going in straight lines then go back in the index till the point
			// the enemy can
			if ((fabs(m_dir.x) > 0.15f && fabs(m_dir.x) < 0.85f) ||
				(fabs(m_dir.y) > 0.15f && fabs(m_dir.y) < 0.85f))
			{

				m_alertPathIndex = m_alertPath->size - 1;
				while (true)
				{
					int2 nodePos = m_alertPath->data[m_alertPathIndex--]->pos;
					if (abs(nodePos.x - pos.x) < 2 || abs(nodePos.y - pos.y) < 2)
					{
						break;
					}
					if (m_alertPathIndex < 0)
					{
						break;
					}
				}

				//reset path
				if (m_alertPathIndex < 0)
				{
					m_alertPathIndex = m_alertPath->size;
					StayInAlert();
					break;

				}

				currentNode = m_alertPath->data[m_alertPathIndex];
				m_dir = static_cast<float2>(currentNode->pos * TILE_SIZE) - m_pos;
				m_dir = normalize(m_dir);

			}


			m_pos += m_dir * ENEMY_SPEED * p_deltaTime;

			int2 dis = currentNode->pos * TILE_SIZE;

			if (fabs(m_pos.x - dis.x) < NEXT_PATROLPOINT_MARGIN &&
				fabs(m_pos.y - dis.y) < NEXT_PATROLPOINT_MARGIN)
			{
				m_pos = currentNode->pos * TILE_SIZE;
				m_alertPathIndex++;
			}

			break;
		}
		case Enemy::EnemyState::evasion:
		{
			if (m_alertPath == nullptr)
			{
				break;
			}
			if (m_alertPathIndex >= m_alertPath->size)
			{
				break;
			}

			int2 pos = { static_cast<int>(m_pos.x) / TILE_SIZE, static_cast<int>(m_pos.y) / TILE_SIZE };
			NavNode* currentNode = m_alertPath->data[m_alertPathIndex];
			m_dir = static_cast<float2>(currentNode->pos * TILE_SIZE) - m_pos;
			m_dir = normalize(m_dir);


			// if the enemy isnt going in straight lines then go back in the index till the point
			// the enemy can
			if ((fabs(m_dir.x) > 0.15f && fabs(m_dir.x) < 0.85f) ||
				(fabs(m_dir.y) > 0.15f && fabs(m_dir.y) < 0.85f))
			{
				printf("this shouldnt happen");

				m_alertPathIndex = m_alertPath->size - 1;
				while (true)
				{
					int2 nodePos = m_alertPath->data[m_alertPathIndex--]->pos;
					if (abs(nodePos.x - pos.x) < 2 || abs(nodePos.y - pos.y) < 2)
					{
						break;
					}
					if (m_alertPathIndex < 0)
					{
						break;
					}
				}

				//reset path
				if (m_alertPathIndex < 0)
				{
					m_alertPathIndex = m_alertPath->size;
					StayInAlert();
					break;

				}

				currentNode = m_alertPath->data[m_alertPathIndex];
				m_dir = static_cast<float2>(currentNode->pos * TILE_SIZE) - m_pos;
				m_dir = normalize(m_dir);

			}

			m_pos += m_dir * ENEMY_SPEED * p_deltaTime;

			int2 dis = currentNode->pos * TILE_SIZE;

			if (fabs(m_pos.x - dis.x) < NEXT_PATROLPOINT_MARGIN &&
				fabs(m_pos.y - dis.y) < NEXT_PATROLPOINT_MARGIN)
			{
				m_pos = currentNode->pos * TILE_SIZE;
				m_alertPathIndex++;
			}


			break;
		}
		case Enemy::EnemyState::goingBackToPatrol:
		{
			if (m_alertPath == nullptr)
			{
				break;
			}
			if (m_alertPathIndex >= m_alertPath->size)
			{
				m_enemyState = EnemyState::normal;
				m_alertPathIndex = 0;
				break;
			}

			int2 pos = { static_cast<int>(m_pos.x) / TILE_SIZE, static_cast<int>(m_pos.y) / TILE_SIZE };
			NavNode* currentNode = m_alertPath->data[m_alertPathIndex];
			m_dir = static_cast<float2>(currentNode->pos * TILE_SIZE) - m_pos;
			m_dir = normalize(m_dir);


			// if the enemy isnt going in straight lines then go back in the index till the point
			// the enemy can
			if ((fabs(m_dir.x) > 0.15f && fabs(m_dir.x) < 0.85f) ||
				(fabs(m_dir.y) > 0.15f && fabs(m_dir.y) < 0.85f))
			{
				printf("this shouldnt happen");

				m_alertPathIndex = m_alertPath->size - 1;
				while (true)
				{
					int2 nodePos = m_alertPath->data[m_alertPathIndex--]->pos;
					if (abs(nodePos.x - pos.x) < 2 || abs(nodePos.y - pos.y) < 2)
					{
						break;
					}
					if (m_alertPathIndex < 0)
					{
						break;
					}
				}

				//reset path
				if (m_alertPathIndex < 0)
				{
					m_alertPathIndex = m_alertPath->size;
					StayInAlert();
					break;

				}

				currentNode = m_alertPath->data[m_alertPathIndex];
				m_dir = static_cast<float2>(currentNode->pos * TILE_SIZE) - m_pos;
				m_dir = normalize(m_dir);

			}

			m_pos += m_dir * ENEMY_SPEED * p_deltaTime;

			int2 dis = currentNode->pos * TILE_SIZE;

			if (fabs(m_pos.x - dis.x) < NEXT_PATROLPOINT_MARGIN &&
				fabs(m_pos.y - dis.y) < NEXT_PATROLPOINT_MARGIN)
			{
				m_pos = currentNode->pos * TILE_SIZE;
				m_alertPathIndex++;
			}


			break;
		}
		case Enemy::EnemyState::normal:
		default:
		{
			if (m_NextNode == nullptr) { return; }

			Instruction currentInstruction = m_ptrArrayPatrolRoute->instructions[m_patrolIndex];
			//float2 dir = { 0,0 };

			switch (currentInstruction.enemyInstruction)
			{
			case Enemy::EnemyInstructions::waitUp:
			case Enemy::EnemyInstructions::walkUp:
			{
				m_dir = { 0,-1 };
				break;
			}
			case Enemy::EnemyInstructions::waitRight:
			case Enemy::EnemyInstructions::walkRight:
			{
				m_dir = { 1,0 };
				break;
			}
			case Enemy::EnemyInstructions::waitDown:
			case Enemy::EnemyInstructions::walkDown:
			{
				m_dir = { 0,1 };
				break;
			}
			case Enemy::EnemyInstructions::waitLeft:
			case Enemy::EnemyInstructions::walkLeft:
			{
				m_dir = { -1,0 };
				break;
			}
			case Enemy::EnemyInstructions::none:
			default:
				break;
			}

			m_pos += m_dir * ENEMY_SPEED * p_deltaTime;

			int2 dis = currentInstruction.destination * TILE_SIZE;

			if (fabs(m_pos.x - (dis.x)) < NEXT_PATROLPOINT_MARGIN &&
				fabs(m_pos.y - (dis.y)) < NEXT_PATROLPOINT_MARGIN)
			{
				printf("old des = %d %d \n", currentInstruction.destination.x, currentInstruction.destination.y);

				m_pos = static_cast<float2>(dis);

				m_patrolIndex++;
				if (m_patrolIndex >= m_patrolSize) m_patrolIndex = 0;
				m_lastNode = m_NextNode;
				m_NextNode = GetNextNodeNavNode();
				currentInstruction = m_ptrArrayPatrolRoute->instructions[m_patrolIndex];
				printf("new des = %d %d \n", currentInstruction.destination.x, currentInstruction.destination.y);
			}
			break;
		}
	}
	
}

void Enemy::Render()
{
	if (!m_isAlive) { return; };

	if (m_ptrSpritesheet == nullptr) { printf("error:spritesheet isnt initialized properly, in Enemy::Render\n"); return; }

	float2 screenSpace = m_pos - *m_camPos;


	if (screenSpace.x > ROOM_SIZE_IN_PIXELS || screenSpace.y > ROOM_SIZE_IN_PIXELS || screenSpace.x <= 0 || screenSpace.y <= 0) return;

	animFramesEnemy frame = {};

	if (m_enemyState == Enemy::EnemyState::normal)
	{
		switch (m_ptrArrayPatrolRoute->instructions[m_patrolIndex].enemyInstruction)
		{
			case Enemy::EnemyInstructions::waitUp:
			case Enemy::EnemyInstructions::walkUp:
			{
				frame = frameData[static_cast<int>(EnemyAnimIndexes::walkUp)];
				break;
			}

			case Enemy::EnemyInstructions::waitRight:
			case Enemy::EnemyInstructions::walkRight:
			{
				frame = frameData[static_cast<int>(EnemyAnimIndexes::walkRight)];
				break;
			}

			case Enemy::EnemyInstructions::waitDown:
			case Enemy::EnemyInstructions::walkDown:
			{
				frame = frameData[static_cast<int>(EnemyAnimIndexes::walkDown)];
				break;
			}

			case Enemy::EnemyInstructions::waitLeft:
			case Enemy::EnemyInstructions::walkLeft:
			{
				frame = frameData[static_cast<int>(EnemyAnimIndexes::walkLeft)];
				break;
			}

			case Enemy::EnemyInstructions::none:
			default:
			break;
		}
	}
	else
	{
		bool isMoving = (!(!m_dir.x && !m_dir.y));

		if (isMoving)
		{
			if (fabs(m_dir.x) > fabs(m_dir.y))
			{
				if (m_dir.x > 0) // enemy is looking right
				{
					frame = frameData[static_cast<int>(EnemyAnimIndexes::walkRight)];
				}
				else // enemy is looking left
				{
					frame = frameData[static_cast<int>(EnemyAnimIndexes::walkLeft)];
				}
			}
			else
			{
				if (m_dir.y > 0) // enemy is looking up
				{
					frame = frameData[static_cast<int>(EnemyAnimIndexes::walkDown)];
				}
				else // enemy is looking down
				{
					frame = frameData[static_cast<int>(EnemyAnimIndexes::walkUp)];
				}
			}
		}
		else
		{
			if (m_enemyState == Enemy::EnemyState::alert)
			{
				m_dir = normalize(m_ptrEnemyManger->GetPlayerPos() - (m_pos / TILE_SIZE));
			}
			else
			{
				m_dir = m_lastSeenPlayer;
			}
			if (fabs(m_dir.x) > fabs(m_dir.y))
			{
				if (m_dir.x > 0) // enemy is looking right
				{
					frame = frameData[static_cast<int>(EnemyAnimIndexes::shootRight)];
				}
				else // enemy is looking left
				{
					frame = frameData[static_cast<int>(EnemyAnimIndexes::shootLeft)];
				}
			}
			else
			{
				if (m_dir.y > 0) // enemy is looking up
				{
					frame = frameData[static_cast<int>(EnemyAnimIndexes::shootDown)];
				}
				else // enemy is looking down
				{
					frame = frameData[static_cast<int>(EnemyAnimIndexes::shootUp)];
				}
			}
		}

	}


	if (frame.frameCount < m_animIndex) m_animIndex = 0;
	int usedFrame;
	if (frame.frameCount == 1) usedFrame = frame.startFrame;
	else usedFrame = (frame.startFrame + static_cast<int>(m_animIndex));
	m_ptrSpritesheet->renderFromScaled({ usedFrame , 0 }, m_pos - *m_camPos, ENEMY_RENDER_SIZE);
	m_lastUsedFrame = usedFrame;



}

void Enemy::UpdateInstructions()
{
	if (!m_isAlive) { return; };

	// Start with the current node
	NavNode* currentNode = m_lastNode;
	NavNode* nextNode = nullptr;

	for (size_t i = 0; i < m_ptrArrayPatrolRoute->instructionCount; i++)
	{
		if (currentNode == nullptr)
		{
			return;
		}

		switch (m_ptrArrayPatrolRoute->instructions[i].enemyInstruction)
		{
			case Enemy::EnemyInstructions::waitUp:
			case Enemy::EnemyInstructions::walkUp:
			{

				nextNode = currentNode->t;
				break;
			}
			case Enemy::EnemyInstructions::waitRight:
			case Enemy::EnemyInstructions::walkRight:
			{
				nextNode = currentNode->r;
				break;
			}
			case Enemy::EnemyInstructions::waitDown:
			case Enemy::EnemyInstructions::walkDown:
			{
				nextNode = currentNode->b;
				break;
			}
			case Enemy::EnemyInstructions::waitLeft:
			case Enemy::EnemyInstructions::walkLeft:
			{
				nextNode = currentNode->l;
				break;
			}
			case Enemy::EnemyInstructions::none:
			default:
			break;
		}

		if (nextNode != nullptr)
		{
			m_ptrArrayPatrolRoute->instructions[i].destination = nextNode->pos;
		}
		else
		{
			return;
		}
		currentNode = nextNode;


	}



}
