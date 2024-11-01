#ifndef SPRITESHEET
#define SPRITESHEET

class SpriteSheet
{
	public:
	SpriteSheet();
	SpriteSheet(int2 p_size, char* p_spritesheetImage, Surface* p_ptrScreen);
	~SpriteSheet();

	// this code is based on sprite::draw
	void renderFrom(int2 p_index, float2 p_pos);
	void renderFromScaled(int2 p_index, float2 p_pos, int2 p_size);
	Surface* getScaledSprite(int2 p_index, int2 p_size);

	const Surface* LookAtUsedSurf() const { return m_ptrSheet; };

	bool initialized = false;
	// m_rows and m_collums should prob be changed to private set public get

	private:
	// m_ptrScreen is a ptr to the screen its a ptr instead of a 
	// ref because SpriteSheet has a default contructor
	Surface* m_ptrScreen; 
	Surface* m_ptrSheet = nullptr;
	int2 m_size;



};

#endif // !SPRITESHEET
