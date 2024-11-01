#ifndef PISTOL
#define PISTOL

#include "baseEquippable.h"
#include "basePickupable.h"

class Projectile;

class Pistol : public baseWeapon
{
	public:
	Pistol(int2 p_roomIndex, SpriteSheet* p_spritesheet, RectInt p_posInRoom);
	//void RenderIcon(int2 p_pos, int2 p_scale) override;
	void Attack(float2 p_pos, float2 p_dir, Projectile* p_usedBullet) const override;
	void Update(float p_deltatime) override { (void)p_deltatime; };
	void Render() override;
	void RenderIcon(int2 p_pos, int2 p_scale) const override;

	private:
	//uint m_ammo;


};


#endif // !PISTOL
