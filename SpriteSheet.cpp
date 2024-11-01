#include "precomp.h"
#include "SpriteSheet.h"
#include <renderingConst.h>


SpriteSheet::SpriteSheet()
{
	m_size = { 0,0 };
	m_ptrScreen = nullptr;
}

SpriteSheet::SpriteSheet(int2 p_size, char* p_spritesheetImage, Surface* p_ptrScreen)
{
	m_size = p_size;
	m_ptrSheet = new Surface(p_spritesheetImage);
	m_ptrScreen = p_ptrScreen;
	initialized = true;
}

SpriteSheet::~SpriteSheet()
{
	//if (m_ptrSheet != nullptr) delete m_ptrSheet;
}

void SpriteSheet::renderFrom(int2 p_index, float2 p_pos)
{
	if (m_size.x == 0 || m_size.y == 0) {printf("error:spritesheet isnt initialized properly, in SpriteSheet::renderFrom\n"); return;}

	if (p_index.x >= m_size.x) return;
	if (p_index.y >= m_size.y) return;

	int spriteSizeX = static_cast<int>(m_ptrSheet->width / m_size.x);
	int spriteSizeY = static_cast<int>(m_ptrSheet->height / m_size.y);

	int2 posFloored = { static_cast<int>(p_pos.x), static_cast<int>(p_pos.y) };

	uint* ptrScreenPixels = m_ptrScreen->pixels;
	uint* ptrSpriteSheetPixels = m_ptrSheet->pixels;


	for (int i = 0; i < spriteSizeX; i++)
	{
		if (i + posFloored.x >= m_ptrScreen->width)
		{
			continue;
		}

		for (int j = 0; j < spriteSizeY; j++)
		{
			if (j + posFloored.y >= m_ptrScreen->height)
			{
				continue;
			}

			uint pixel = ptrSpriteSheetPixels[((p_index.x * spriteSizeX) + i) + ((p_index.y * spriteSizeY) + j) * m_ptrSheet->width];

			if ((pixel & TRANSPARENCY_MASK) == TRANSPARENCY_MASK)
			{
				continue;
			}

			ptrScreenPixels[(posFloored.x + i) + (posFloored.y + j) * m_ptrScreen->width] = pixel;

		}
	}
}

//needs refactoring
void SpriteSheet::renderFromScaled(int2 p_index, float2 p_pos, int2 p_size)
{
	if (m_size.x == 0 || m_size.y == 0)
	{
		printf("error:spritesheet isnt initialized properly, in SpriteSheet::renderFrom\n");
		return;
	}

	if (p_index.x >= m_size.x) return;
	if (p_index.y >= m_size.y) return;

	int2 posFloored = { static_cast<int>(p_pos.x), static_cast<int>(p_pos.y) };

	// if spritesheet is out of bounds then return ealry
	if (p_index.x + posFloored.x < 0 ||  p_index.x > m_ptrScreen->width) return;
	if (p_index.y + posFloored.y < 0 || p_index.y > m_ptrScreen->height) return;

	int spriteSizeX = static_cast<int>(m_ptrSheet->width / m_size.x);
	int spriteSizeY = static_cast<int>(m_ptrSheet->height / m_size.y);

	
	float scaleX = static_cast<float>(spriteSizeX) / static_cast<float>(p_size.x);
	float scaleY = static_cast<float>(spriteSizeY) / static_cast<float>(p_size.y);

	uint* ptrScreenPixels = m_ptrScreen->pixels;
	uint* ptrSpriteSheetPixels = m_ptrSheet->pixels;

	for (size_t i = 0; i < p_size.x; i++)
	{
		if (i + posFloored.x >= m_ptrScreen->width)
		{
			continue;
		}

		for (size_t j = 0; j < p_size.y; j++)
		{
			if (j + posFloored.y >= m_ptrScreen->height)
			{
				continue;
			}

			int scrX = static_cast<int>(scaleX * i) + p_index.x * spriteSizeX;
			int scrY = static_cast<int>(scaleY * j) + p_index.y * spriteSizeY;

			uint pixel = ptrSpriteSheetPixels[scrX + scrY * m_ptrSheet->width];

			if ((pixel & TRANSPARENCY_MASK) == TRANSPARENCY_MASK && pixel != 0xffffff)
			{
				continue;
			}

			ptrScreenPixels[(posFloored.x + i) + (posFloored.y + j) * m_ptrScreen->width] = pixel;

		}
	}
}

Surface* SpriteSheet::getScaledSprite(int2 p_index, int2 p_size)
{
	if (m_size.x == 0 || m_size.y == 0)
	{
		printf("error: spritesheet isn't initialized properly, in SpriteSheet::getScaledSprite\n");
		return nullptr;
	}

	if (p_index.x >= m_size.x || p_index.y >= m_size.y)
	{
		printf("error: sprite index out of bounds, in SpriteSheet::getScaledSprite\n");
		return nullptr;
	}

	int spriteSizeX = static_cast<int>(m_ptrSheet->width / m_size.x);
	int spriteSizeY = static_cast<int>(m_ptrSheet->height / m_size.y);

	// Create a new surface to store the scaled sprite
	Surface* scaledSurface = new Surface(p_size.x, p_size.y);

	// Calculate scaling factors
	float scaleX = static_cast<float>(spriteSizeX) / static_cast<float>(p_size.x);
	float scaleY = static_cast<float>(spriteSizeY) / static_cast<float>(p_size.y);

	// Get the pixel data pointers
	uint* ptrSpriteSheetPixels = m_ptrSheet->pixels;
	uint* ptrScaledSurfacePixels = scaledSurface->pixels;

	// Render the sprite onto the new surface
	for (size_t i = 0; i < p_size.x; i++)
	{
		for (size_t j = 0; j < p_size.y; j++)
		{
			int scrX = static_cast<int>(scaleX * i) + p_index.x * spriteSizeX;
			int scrY = static_cast<int>(scaleY * j) + p_index.y * spriteSizeY;

			uint pixel = ptrSpriteSheetPixels[scrX + scrY * m_ptrSheet->width];

			// Only render non-transparent pixels
			ptrScaledSurfacePixels[i + j * p_size.x] = pixel;
		}
	}

	return scaledSurface;
}


