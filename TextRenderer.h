#ifndef TEXT_RENDERER
#define TEXT_RENDERER

#include "SpriteSheet.h"

class TextRenderer
{
	public:
	TextRenderer(Surface* p_ptrScreen);
	void RenderText(int2 p_pos, int2 p_charSize, char* p_text);
	void RenderText(int2 p_pos, int2 p_charSize, char p_text);

	private:

	int2 GetIndexOfChar(char p_char);
	SpriteSheet m_spritesheet;
};



#endif // !TEXT_RENDERER