#ifndef ENEMY
#define ENEMY

#include "SpriteSheet.h"
#include "BaseActor.h"
#include "EnemyManager.h"
#include "MapManager.h"

// Forward declaration of Enemy and NavNode
struct NavNode;
class EnemyManager;
class Enemy;

class Enemy : public BaseActor
{
	public:

	enum class EnemyInstructions
	{
		none,
		waitUp,
		walkUp,
		waitRight,
		walkRight,
		waitDown,
		walkDown,
		waitLeft,
		walkLeft,
	};

	struct Instruction
	{
		int2 destination;
		EnemyInstructions enemyInstruction;
	};

	struct InstructionsStruct
	{
		Instruction* instructions;
		int instructionCount;
	};


	struct InstructionsStruct;

	Enemy(SpriteSheet* p_ptrSpritesheet, const int2* p_camPos, InstructionsStruct* p_ptrArrayPatrolRoute, int2 p_startPos, int2& p_mapRoomSize);

	const RectInt GetRectFromActor() const override;

	void TakeDamage(int p_amount) override;
	void Respawn();
	bool IsAlive() const{ return m_isAlive; };
	bool IsStunned() const { return (m_stunTimer > 1 && m_stunTimer < STUN_DUR); };

	// !!! this needs to be updated
	Surface* LookAtSurface() const override;


	const float2 GetLookDir() const { return m_dir; };
	const float2 GetPos() const { return m_pos; };
	const int2 GetStartPos() const { return m_startPos; };

	void Update(float p_deltaTime) override;
	void Render() override;
	void SetEnemyManager(EnemyManager* p_ptrEnemyManger) { m_ptrEnemyManger = p_ptrEnemyManger;};
	//void SetEnemyManger(EnemyManager* p_ptrEnemyManger) { m_ptrEnemyManger = p_ptrEnemyManger; };

	InstructionsStruct* m_ptrArrayPatrolRoute;
	void Init(EnemyManager* p_ptrEnemyManger);
	void GoIntoAlert();
	void StayInAlert();
	void GoIntoEvasion();
	void GoBackToPatrol();


	const int2 ENEMY_RENDER_SIZE = { 100, 100 };
	private:

	float2 m_pos;
	int2 m_startPos;
	NavNode* GetNextNodeNavNode();
	void UpdateInstructions();

	void Die() { m_isAlive = false; };

	enum class EnemyAnimIndexes
	{
		walkDown,
		shootDown,
		walkLeft,
		shootLeft,
		walkUp,
		shootUp,
		walkRight,
		shootRight,
	};

	animFramesEnemy frameData[8] =
	{
		{0,2},
		{3,1},
		{4,2},
		{7,1},
		{8,2},
		{11,1},
		{12,2},
		{15,1},
	};

	NavNode* m_lastNode;
	NavNode* m_NextNode;


	DymArr<NavNode*>* m_alertPath = nullptr;

	EnemyManager* m_ptrEnemyManger = nullptr;
	SpriteSheet* m_ptrSpritesheet;
	HumanActorLookDir m_lookingDir, m_walkDir;

	const int2 SPRITESHEET_SIZE = { 16, 1 };
	const int2* m_camPos;
	float m_animIndex = 0;
	int m_patrolIndex = 0, m_patrolSize = 0, m_alertPathIndex = 0, m_lastUsedFrame = 0;
	const float NEXT_PATROLPOINT_MARGIN = 5.0f;
	const float ENEMY_SPEED = 0.1f;
	int m_health = 60;

	float2 m_dir = { 0,0 };
	float2 m_lastSeenPlayer = { 0,0 };

	float const SHOOT_COOL_DOWN = 750;
	float m_shootTimer = SHOOT_COOL_DOWN;
	float const STUN_DUR = 1500;
	float m_stunTimer = STUN_DUR;

	int2& m_mapRoomSize;

	enum class EnemyState
	{
		alert,
		evasion,
		goingBackToPatrol,
		normal
	};

	EnemyState m_enemyState = EnemyState::normal;

	// if loopPatrol is fale the enemy will go from front to back and back to front
	bool isShooting = false;
	bool loopPatrol = false;
	bool m_isAlive = true;


};


#endif // !ENEMY
