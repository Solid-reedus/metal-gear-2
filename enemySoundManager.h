#ifndef ENEMY_SOUND_MANAGER
#define ENEMY_SOUND_MANAGER

#include <Audio/Sound.hpp>
#include <Audio/Waveform.hpp>


class EnemySoundManager
{
	public:
	enum EnemySound
	{
		question,
		alert,
	};


	EnemySoundManager();
	//EnemySoundManager();

	void Play(EnemySound p_sound);


	private:

	const char* m_sfxNames[2] =
	{
		"assets/enemyHuh.mp3",
		"assets/enemyAlert.mp3",
	};

	Audio::Sound m_sfx[2];
	Audio::Sound* m_currentSfx = nullptr;

};

#endif // !ENEMY_SOUND_MANAGER
