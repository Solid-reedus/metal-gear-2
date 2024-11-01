#ifndef UI_MANAGER
#define UI_MANAGER

/*
	!!!! delete this class

*/


class Player;

class UIManger
{


	public:
	UIManger();
	~UIManger();

	void Render();

	private:
	Player* m_ptrPlayer;
	Surface* m_screen;


};


#endif // !UI_MANAGER



