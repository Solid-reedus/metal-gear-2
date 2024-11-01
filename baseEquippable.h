#ifndef BASE_EQUIPPABLE
#define BASE_EQUIPPABLE

#include "basePickupable.h"

class SpriteSheet;
class Projectile;

class baseWeapon : public BasePickupable
{
	public:
	virtual ~baseWeapon() = default;
	//virtual void RenderIcon(int2 p_pos, int2 p_scale) = 0;
	virtual void Attack(float2 p_pos, float2 m_dir, Projectile* p_usedProjectile) const = 0;
	int m_ammo = 0;

	protected:
	char* m_description;
	//SpriteSheet* m_spritesheet;
	int2 m_index; // , m_renderSize;
};

class Baseitem : public BasePickupable
{
	public:
	virtual ~Baseitem() = default;
	virtual void Render(int2 p_pos, int2 p_scale) = 0;
	virtual void Use() = 0;

	protected:
	char* m_description;
	SpriteSheet* m_spritesheet;
	int2 m_index;

};


#endif // !BASE_EQUIPPABLE
