#ifndef PLAYER
#define PLAYER

#include "BaseActor.h"
#include "renderingConst.h"
#include "MapManager.h"
#include "commonTypes.h"
#include "game.h"
#include "radioManager.h"

class baseWeapon;
class SpriteSheet;
class Baseitem;
class Projectile;

class Player : public BaseActor
{
	public:

	enum RadioFlags
	{
		fenceRing = 1 << 0,
		insideRing = 1 << 1,
	};

	Player(float2 p_startPos, Surface* p_ptrScreen, MapManager* p_ptrMapManager, bool p_playIntro, int p_levelIndex);
	~Player();

	//this gets called each update
	void Update(float p_deltaTime) override;
	void Render() override;
	const RectInt GetRectFromActor() const override;
	const Surface* LookAtSurface() const override; 

	//this gets called when a key is down
	void KeyDown(int p_keyDown);
	void KeyUp(int p_keyUp);

	void KeyDownRadio(int p_keyDown);
	const uchar GetRadioIndex() const { return m_radioIndex; };

	void KeyDownInventory(int p_keyDown, bool p_isInWeaponSelect);
	const int2 GetInventoryIndex() const { return m_inventoryIndex; };

	const int2* GetCamPos() const {return &m_camPos;};
	const float2& GetPLayerWorldPos() const {return m_worldPos;};
	const float2 GetScreenPos();
	const bool PlayerIsInRoom() const { return m_isInRoom; };
	const bool PlayerIsCrouching() const { return (m_playerState & playerCrouching); }
	bool ShowRadioFLags(); 

	DymArr<const BasePickupable*>* GetRenderedItems(bool p_getWeapon) const;

	const DymArr<Projectile*>* GetProjectilesPool() const{ return m_projectilePool; };
	const RectInt GetRectFromPlayer() const;

	void AddWeapon(baseWeapon* p_newWeapon);

	Tmpl8::Game::PlayerLoadData GetPlayerData();
	void SetPlayer(Tmpl8::Game::PlayerLoadData p_loadData, PickupManager* p_pickupManager);

	void TakeDamage(int p_amount) override;
	const bool CheckIfPlayerIsDead() const { return (m_health < 1); };
	const bool CheckIfPlayerIsPunching() const { return (m_punchCoolDown != 0); };
	const float2 LookAtPlayerWalkDir() const;
	const bool PlayerWon() const { return m_playerWon; };

	const RadioManager::voiceLine* LookAtCurrentVoiceLine() const;

	const baseWeapon* LookAtCurrentEquipedWeapon() const { return m_currentWeapon; };


	const float LookAtHealthpercentage() const;

	void ChangeCamPos() { m_camPos = m_ptrMapManager->GetCurrentRoom();  m_camPos *= ROOM_SIZE_IN_PIXELS; }
	private:
	
	bool NextStepIsValid(float2 p_currentPos, float2 p_nextStep) const;
	void CheckTeleportPlayer();
	void TelePortPlayer();

	enum class animframeName
	{
		standUp    = 0,
		standDown  = 1,
		standLeft  = 2,
		standRight = 3,

		walkUp     = 4,
		walkDown   = 5,
		walkLeft   = 6,
		walkRight  = 7,

		standGunUp    = 8,
		standGunDown  = 9,
		standGunLeft  = 10,
		standGunRight = 11,

		walkGunUp    = 12,
		walkGunDown  = 13,
		walkGunLeft  = 14,
		walkGunRight = 15,

		crouchStartUp    = 16,
		crouchStartDown  = 17,
		crouchStartLeft  = 18,
		crouchStartRight = 19,

		crouchIdleUp    = 20,
		crouchIdleDown  = 21,
		crouchIdleLeft  = 22,
		crouchIdleRight = 23,

		crouchingUp    = 24,
		crouchingDown  = 25,
		crouchingLeft  = 26,
		crouchingRight = 27,

		punchUp    = 28,
		punchDown  = 29,
		punchLeft  = 30,
		punchRight = 31,

		introAnim = 32,
		box = 33,


	};

	enum PlayerStateFlags
	{
		playerInControl		= 1 << 0,
		playerIsWalking		= 1 << 1,
		playerCrouching		= 1 << 2,
		playerHasGun		= 1 << 3,
		playerIsInAnimation = 1 << 4,
		playerIsMustCrouch  = 1 << 5,
	};


	float2 m_pos, m_dir, m_worldPos;
	int2 m_camPos;
	const int2 SPRITESHEET_SIZE = { 7, 9 };
	SpriteSheet* m_ptrSpritesheet;
	MapManager* m_ptrMapManager;
	RadioManager* m_radioManager;
	const RadioManager::conversationStruct* m_currentConversation = nullptr;
	int conversationIndex = 0;

	const float PLAYER_SPEED = 0.2f;
	int2 m_roomSize = { 0,0 };
	int2 m_currentRoom = { 0, 0 };
	int2 m_lastRenderIndex = { 0, 0 };

	baseWeapon* m_currentWeapon = nullptr;
	Baseitem* m_currentItem = nullptr;

	baseWeapon* m_weaponInventory[6] = { nullptr };
	Baseitem* m_itemInventory[6] = { nullptr };

	DymArr<Projectile*>* m_projectilePool;
	int projectileIndex = 0;

	int m_levelIndex = 0;

	// m_lookingDir is used for what animation to use
	// m_walkDir is used for determining what dir the player is going 
	HumanActorLookDir m_lookingDir, m_walkDir;

	int m_health;
	const int MAX_HEALTH = 600;
	int m_playerState = 0;
	float m_animIndex;
	int2 m_inventoryIndex = { 0,0 };

	float m_introAnimCountDown = 0;
	const float INTRO_ANIM_DURATION = 7000;

	float m_punchCoolDown = 0;
	const float PUNCH_COOL_DUR = 300;

	const int2 PLAYER_SIZE = { 120, 100 };
	const int2 PLAYER_INVENTORY_MAX_INDEX = { 1, 2 };

	const struct animFramesPlayer frameData[34]
	{
		{{1,0}, 1}, // standUp   
		{{1,1}, 1}, // standDown 
		{{1,2}, 1}, // standLeft 
		{{1,3}, 1}, // standRight

		{{0,0}, 3}, // walkUp   
		{{0,1}, 3}, // walkDown 
		{{0,2}, 3}, // walkLeft 
		{{0,3}, 3}, // walkRight

		{{4,0}, 1}, // standGunUp   
		{{4,1}, 1}, // standGunDown 
		{{4,2}, 1}, // standGunLeft 
		{{4,3}, 1}, // standGunRight

		{{3,0}, 3}, // walkGunUp   
		{{3,1}, 3}, // walkGunDown 
		{{3,2}, 3}, // walkGunLeft 
		{{3,3}, 3}, // walkGunRight

		{{0,4}, 1}, // crouchStartUp    
		{{0,5}, 1}, // crouchStartDown  
		{{0,6}, 1}, // crouchStartLeft  
		{{0,7}, 1}, // crouchStartRight 

		{{2,4}, 1}, // crouchIdleUp   
		{{2,5}, 1}, // crouchIdleDown 
		{{2,6}, 1}, // crouchIdleLeft 
		{{2,7}, 1}, // crouchIdleRight

		{{1,4}, 3}, // crouchingUp    
		{{1,5}, 3}, // crouchingDown  
		{{1,6}, 3}, // crouchingLeft  
		{{1,7}, 3}, // crouchingRight 

		{{6,0}, 1}, // punchUp    
		{{6,1}, 1}, // punchDown  
		{{6,2}, 1}, // punchLeft  
		{{6,3}, 1}, // punchRight 

		{{0,8}, 5}, // introAnim
		{{3,4}, 1}, // box
	};

	static const int PLAYER_INTRO_FRAMES = 25;

	const struct int2 playerIntroFrameData[PLAYER_INTRO_FRAMES]
	{
		{0,8},
		{1,8},
		{2,8},
		{3,8},
		{4,8},
		{1,6},
		{2,6},
		{3,6},
		{1,6},
		{2,6},
		{3,6},
		{1,6},
		{2,6},
		{3,6},
		{1,6},
		{2,6},
		{3,6},
		{1,6},
		{2,6},
		{3,6},
		{1,6},
		{2,6},
		{3,6},
		{0,6},
		{0,6},
	};

	int m_radioflags = fenceRing | insideRing;
	uchar m_radioIndex = 85;
	bool m_isInRoom = false;
	bool m_playerWon = false;



};


#endif // !PLAYER
