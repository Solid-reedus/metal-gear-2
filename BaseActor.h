#ifndef BASE_ACTOR
#define BASE_ACTOR


#include "commonTypes.h"


// baseActor is a abstract class used for all actors in a scene
class BaseActor
{
	public:
	virtual void Render() = 0;
	virtual void Update(float p_deltaTime) = 0;
	virtual void TakeDamage(int p_amount) = 0;
	~BaseActor() = default;
	
	virtual const RectInt GetRectFromActor() const = 0;
	virtual const Surface* LookAtSurface() const = 0;

	struct animFramesPlayer
	{
		int2 startFrame;
		uchar frameCount;
	};
	struct animFramesEnemy
	{
		uchar startFrame;
		uchar frameCount;
	};
	enum HumanActorLookDir
	{
		dirNone = 0,
		dirUp = GLFW_KEY_W,
		dirRight = GLFW_KEY_D,
		dirDown = GLFW_KEY_S,
		dirLeft = GLFW_KEY_A,
	};

};


#endif // !BASE_ACTOR
