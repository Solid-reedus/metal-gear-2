#ifndef WIN_LOSE_SCENE
#define WIN_LOSE_SCENE

#include "baseScene.h"

class TextRenderer;
class SaveLoadManager;

class WinLoseScence : public BaseScene
{
	public:
	WinLoseScence() {};
	~WinLoseScence();

	void Init(Tmpl8::Game* p_game, Surface* p_screen) override;
	void Update(float p_deltaTime) override;
	void KeyUp(int p_keyUp) override { (void)p_keyUp; };
	void KeyDown(int p_keyDown) override;

	void SetScreen(bool p_won);

	private:
	TextRenderer* m_textRenderer = nullptr;
	SaveLoadManager* m_saveLoadManager = nullptr;
	char* sceneText = "";

	int m_menuIndex = 0;
	const int MAX_INDEX = 2;

};


#endif // !WIN_LOSE_SCENE
