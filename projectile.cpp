#include "precomp.h"
#include "projectile.h"
#include "renderingConst.h"



Projectile::Projectile(int2 p_bulletSize, uint p_bulletColor, Surface* p_screen)
{
	m_surface = new Surface(p_bulletSize.x, p_bulletSize.y);
	m_surface->Clear(p_bulletColor);
	m_dir = { 0,0 };
	m_pos = { 0,0 };
	m_screen = p_screen;
}

void Projectile::Update(float p_deltatime)
{
	if (!m_active)
	{
		return;
	}

	m_pos += m_dir * PROJECTILE_SPEED * p_deltatime;

	if (m_pos.x < MAP_RENDERING_START || m_pos.x > ROOM_SIZE_IN_PIXELS ||
		m_pos.y < MAP_RENDERING_START || m_pos.y > ROOM_SIZE_IN_PIXELS)
	{
		m_active = false;
	}

	
}

void Projectile::Shoot(float2 p_pos, float2 p_dir)
{
	m_dir = p_dir;
	m_pos = p_pos;
	m_active = true;
}

void Projectile::Render()
{
	m_surface->CopyTo(m_screen, static_cast<int>(m_pos.x), static_cast<int>(m_pos.y));
}

void Projectile::DestroySelf()
{
	m_active = false;
}
