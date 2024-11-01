#ifndef TILE_SHEET
#define TILE_SHEET

class Tilesheet
{

	public:
	Tilesheet();
	Tilesheet(int2 p_size, char* p_tilesheetDir, Surface* p_screen);
	void Draw(int2 p_pos, int2 p_index);
	bool Initialized();

	Surface* GetSpecificTile(int2 p_index) const { return m_ptrArrayTiles[p_index.x + p_index.y * m_size.x]; };

	int2 m_size;

	private:

	Surface** m_ptrArrayTiles = nullptr;
	Surface* m_screen = nullptr;



};


#endif // !TILE_SHEET
