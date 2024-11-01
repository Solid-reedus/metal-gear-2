#ifndef MAIN_MENU_SCENE
#define MAIN_MENU_SCENE

#include "baseScene.h"

class TextRenderer;
class SaveLoadManager;


class MainMenuScene : public BaseScene
{
	public:
	MainMenuScene();
	~MainMenuScene();
	void Init(Tmpl8::Game* p_game, Surface* p_screen) override;
	void Update(float p_deltaTime) override;
	void KeyUp(int p_keyUp) override {(void)p_keyUp;};
	void KeyDown(int p_keyDown) override;

	private:

	int m_menuIndex = 0;
	const int MAX_INDEX = 1;

	TextRenderer* m_textRenderer = nullptr;
	SaveLoadManager* m_saveLoadManager = nullptr;
};


#endif // !MAIN_MENU_SCENE
