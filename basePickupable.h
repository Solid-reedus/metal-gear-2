#ifndef PICKUPABLE
#define	PICKUPABLE

#include "commonTypes.h"

class SpriteSheet;

class BasePickupable
{
	public:

	enum pickUpflag
	{
		pickupInTruck = 1 << 0,
		pickupRespawn  = 1 << 1,
	};

	virtual ~BasePickupable() = default;
	virtual void Render() = 0;
	virtual void RenderIcon(int2 p_pos, int2 p_scale) const = 0;
	virtual void Update(float p_deltatime) = 0;
	const RectInt& GetRect() const { return m_rect; };
	const int2& GetRoomIndex() const { return m_roomIndex; };
	const int& GetFlags() const { return m_flags; };

	protected:
	SpriteSheet* m_spritesheet;
	RectInt m_rect;
	int2 m_roomIndex;
	int m_flags;

};


#endif // !PICKUPABLE
