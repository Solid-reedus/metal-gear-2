#include "precomp.h"
#include "TextRenderer.h"

TextRenderer::TextRenderer(Surface* p_ptrScreen)
{
	m_spritesheet = SpriteSheet({ 13,7 }, "assets/whiteFont_cyanMask.png", p_ptrScreen);
}

void TextRenderer::RenderText(int2 p_pos, int2 p_charSize, char* p_text)
{
	for (char* c = p_text; *c != '\0'; ++c)
	{
		if (*c == ' ')
		{
			p_pos.x += p_charSize.x;
			continue;
		}
		m_spritesheet.renderFromScaled(GetIndexOfChar(*c), p_pos, p_charSize);
		p_pos.x += p_charSize.x;
	}
}


void TextRenderer::RenderText(int2 p_pos, int2 p_charSize, char p_text)
{
	m_spritesheet.renderFromScaled(GetIndexOfChar(p_text), p_pos, p_charSize);
}

int2 TextRenderer::GetIndexOfChar(char p_char)
{
	switch (p_char)
	{
	case 'a': return { 0, 0 };
	case 'b': return { 1, 0 };
	case 'c': return { 2, 0 };
	case 'd': return { 3, 0 };
	case 'e': return { 4, 0 };
	case 'f': return { 5, 0 };
	case 'g': return { 6, 0 };
	case 'h': return { 7, 0 };
	case 'i': return { 8, 0 };
	case 'j': return { 9, 0 };
	case 'k': return { 10, 0 };
	case 'l': return { 11, 0 };
	case 'm': return { 12, 0 };

	case 'n': return { 0, 1 };
	case 'o': return { 1, 1 };
	case 'p': return { 2, 1 };
	case 'q': return { 3, 1 };
	case 'r': return { 4, 1 };
	case 's': return { 5, 1 };
	case 't': return { 6, 1 };
	case 'u': return { 7, 1 };
	case 'v': return { 8, 1 };
	case 'w': return { 9, 1 };
	case 'x': return { 10, 1 };
	case 'y': return { 11, 1 };
	case 'z': return { 12, 1 };

	case '1': return { 0, 4 };
	case '2': return { 1, 4 };
	case '3': return { 2, 4 };
	case '4': return { 3, 4 };
	case '5': return { 4, 4 };
	case '6': return { 5, 4 };
	case '7': return { 6, 4 };
	case '8': return { 7, 4 };
	case '9': return { 8, 4 };
	case '0': return { 9, 4 };

	case '.': return { 2, 6 };
	case '/': return { 0, 5 };
	case ':': return { 3, 5 };

	default: return { -1, -1 }; // For unsupported characters
	}
}

