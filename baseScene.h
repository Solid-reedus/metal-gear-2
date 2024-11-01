#ifndef BASE_SCENE
#define BASE_SCENE

#include "game.h"

class BaseScene
{
	public:
	virtual ~BaseScene() = default;
	virtual void Init(Tmpl8::Game* p_game, Surface* p_screen) = 0;
	virtual void Update(float p_deltaTime) = 0;
	virtual void KeyUp(int p_keyUp) = 0;
	virtual void KeyDown(int p_keyDown) = 0;

	protected:
	Tmpl8::Game* m_game = nullptr;
	Surface* m_screen = nullptr;
};


#endif // !BASE_SCENE
