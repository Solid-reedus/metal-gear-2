#ifndef COMMON_TYPES
#define COMMON_TYPES


template <typename T>
struct DymArr
{
	T* data;
	int size;

	~DymArr()
	{
		delete[] data;
	}
};

struct RectInt
{
	int x, y, w, h;

	bool Overlaps(const RectInt& other) const
	{
		if (x + w <= other.x || other.x + other.w <= x) return false;
		if (y + h <= other.y || other.y + other.h <= y) return false;
		return true;
	}

};


#endif // !COMMON_TYPES