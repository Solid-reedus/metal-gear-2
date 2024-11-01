#ifndef PROJECTILE
#define PROJECTILE

#include "commonTypes.h"

class Projectile
{
	public:
	Projectile(int2 p_bulletSize, uint p_bulletColor, Surface* p_screen);

	void Update(float p_deltatime);
	void Shoot(float2 p_pos, float2 p_dir);
	void Render();
	//const bool IsActive() const { return m_active; };
	const RectInt GetRectBasedOnSurface() const { return {static_cast<int>(m_pos.x), static_cast<int>(m_pos.y),  m_surface->width, m_surface->height }; };
	const Surface* LookAtSurface() const { return m_surface; };

	bool m_active = false;
	int m_damage = 60;

	private:

	void DestroySelf();

	float2 m_dir, m_pos;
	const float PROJECTILE_SPEED = 0.2f;
	Surface* m_screen, *m_surface;

};


#endif // !PROJECTILE
