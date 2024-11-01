#include "precomp.h"
#include "TileSheet.h"
#include "renderingConst.h"

Tilesheet::Tilesheet(int2 p_size, char* p_tilesheetDir, Surface* p_screen)
{
	m_screen = p_screen;
	m_size = p_size;

	Surface tilesheet = Surface(p_tilesheetDir);
	int2 tileSize = { tilesheet.width / p_size.x , tilesheet.height / p_size.y };
	m_ptrArrayTiles = new Surface*[p_size.x * p_size.y];

	for (int i = 0; i < p_size.x; i++)
	{
		for (int j = 0; j < p_size.y; j++)
		{
			Surface newSurf = Surface(tileSize.x, tileSize.y);
			Surface* newSurfScaled = new Surface(TILE_SIZE, TILE_SIZE);
			tilesheet.CopyTo(&newSurf, -(i * tileSize.x), -(j * tileSize.y));
			for (size_t h = 0; h < TILE_SIZE * TILE_SIZE; h++)
			{
				newSurfScaled->pixels[h] = newSurf.pixels[static_cast<int>(h / 4)];
			}
			m_ptrArrayTiles[i + j * p_size.x] = newSurfScaled;
		}
	}
}

Tilesheet::Tilesheet()
{
	m_ptrArrayTiles = nullptr;
	m_screen = nullptr;
	m_size = { 0,0 };
}

void Tilesheet::Draw(int2 p_pos, int2 p_index)
{
	m_ptrArrayTiles[p_index.x + p_index.y * m_size.x]->CopyTo(m_screen, p_pos.x, p_pos.y);
}

bool Tilesheet::Initialized()
{
	return !(m_ptrArrayTiles == nullptr || m_screen == nullptr);
}
