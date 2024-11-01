#ifndef ENEMY_MANAGER
#define ENEMY_MANAGER

#include "enemySoundManager.h"
#include "SpriteSheet.h"
#include "Enemy.h"
#include "commonTypes.h"
#include "enemySoundManager.h"

class Enemy;
struct NavMesh;
struct NavNode;
class MapManager;
class LevelAudioManager;
class Projectile;

class EnemyManager
{
	public:

	enum class GameplayState
	{
		normal,
		alert,
		evasion,
		caution,
	};


	EnemyManager(char* p_csvEnemYInstructions, const int2* p_camPos, const float2& p_playerPos, Surface* p_ptrScreen, MapManager* p_mapManger, LevelAudioManager* p_levelAudioManager);
	//~EnemyManager();

	void Update(float p_deltaTime);
	DymArr<Enemy*>* m_ptrArrayEnemies;
	NavNode* GetClosestNavNode(int2 p_pos) const;

    /*
     this will get the closest node based on
     1 is it aligned with one of the enemies axis
     2 would the node cause the enemy to walk troight the wall and if so dont
     if somehow the enemy doesnt find a pos then return nullptr
    */
	NavNode* GetClosestNavNodeWithCol(int2 p_pos, int roomIndex) const;
	// the reason that the dynamic array is returned as a array is that it will otherwise call the deconstructor when called
	// making the data useless
	DymArr<NavNode*>* GetPath(NavNode* p_startNode, NavNode* p_endNode);

	const float2& GetPlayerPos() const { return m_playerPos; };

	const GameplayState LookAtCurrentGameState() const { return m_gameState; };
	const DymArr<Projectile*>* GetProjectilesPool() const { return m_projectilePool; };
	const float LootAtTimer() const { return m_timer; };

	void GoIntoAlert();
	void StayAlert();
	void GoIntoEvasion();
	void GoBackToPatrol();
	void Shoot(float2 p_pos, float2 p_dir);

	private:

	int GetStepCost(NavNode* p_currentNode, NavNode* p_startNode, NavNode* p_endNode);

    // This struct is inspired by balasz [PR Y1]
    class NavNodeQueue
    {
        public:
        NavNodeQueue(uint p_maxSize) : m_maxSize(p_maxSize)
        {
            m_queue = new NavNode * [p_maxSize];
            m_startPitch = 0;
            m_endPitch = 0;
        }

        ~NavNodeQueue()
        {
            delete[] m_queue;
        }

        int Getsize() const
        {
            return m_endPitch - m_startPitch;
        }

        NavNode* GetFirst()
        {
            // Check if we are within bounds to avoid underflow
            if (m_startPitch < m_endPitch)
            {
                return m_queue[m_startPitch++];
            }
            else
            {
                return nullptr; // Return nullptr if queue is empty
            }
        }

        void AddAtEnd(NavNode* p_newNode)
        {
            // Check if adding a new node would exceed the max size
            if (m_endPitch < m_maxSize)
            {
                m_queue[m_endPitch++] = p_newNode;
            }
            else
            {
                // Log an error or handle overflow here
                printf("NavNodeQueue overflow: max size reached\n");
            }
        }

        NavNode** LookAtQueue() {return m_queue;}
        const uint GetStartPitch() const {return m_startPitch;}
        const uint GetEndPitch() const {return m_endPitch;}

        private:
        uint m_startPitch, m_endPitch;
        NavNode** m_queue;
        const uint m_maxSize;
    };


	SpriteSheet m_ptrSpritesheet;
	const int2* m_camPos;
	const float2& m_playerPos;
	NavMesh* m_navmesh;
	LevelAudioManager* m_levelAudioManager;
	MapManager* m_mapManager;
	// this is timer used for all the states
	float m_timer = 0;
	GameplayState m_gameState = GameplayState::normal;
	EnemySoundManager m_enemySoundManager;


	DymArr<Projectile*>* m_projectilePool;
	int projectileIndex = 0;
};


#endif // !1
