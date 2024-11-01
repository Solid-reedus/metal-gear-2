#ifndef RENDERING_CONST
#define RENDERING_CONST

// Cyan transparency mask
constexpr int TRANSPARENCY_MASK = 0x00FFFF;
constexpr int TILE_SIZE = (8 * 2);

constexpr int MAP_RENDERING_START = 50;
constexpr int ROOM_SIZE = 47;
constexpr int ROOM_SIZE_IN_PIXELS = ROOM_SIZE * TILE_SIZE;

constexpr uint LIGHT_GREEN = 0x00853C;
constexpr uint DARK_GREEN = 0x003C1A;
constexpr uint DARK_BLUE = 0x0049DB;


#endif // !RENDERING_CONST
