#include "precomp.h"
#include "pistol.h"
#include "SpriteSheet.h"
#include "projectile.h"

Pistol::Pistol(int2 p_roomIndex, SpriteSheet* p_spritesheet, RectInt p_posInRoom)
{
	m_description = "socom pistol";
	m_index = { 0, 0 };

	m_rect = p_posInRoom;
	m_roomIndex = p_roomIndex;
	m_spritesheet = p_spritesheet;
	m_flags = BasePickupable::pickUpflag::pickupInTruck; // | BasePickupable::pickUpflag::pickupRepawn;
	m_ammo = 50;
}

void Pistol::Attack(float2 p_pos, float2 p_dir, Projectile* p_usedBullet) const
{
	p_usedBullet->Shoot(p_pos, p_dir);
	// add here pistol spef stuff
}



void Pistol::Render()
{
	m_spritesheet->renderFromScaled(m_index, { static_cast<float>(m_rect.x), static_cast<float>(m_rect.y) }, {m_rect.w, m_rect.h});
}

void Pistol::RenderIcon(int2 p_pos, int2 p_scale) const
{
	m_spritesheet->renderFromScaled(m_index, p_pos, p_scale);
}
