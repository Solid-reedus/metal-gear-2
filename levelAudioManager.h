#ifndef LEVEL_AUDIO_MANAGER
#define LEVEL_AUDIO_MANAGER

#include <Audio/Sound.hpp>
#include <Audio/Waveform.hpp>

class LevelAudioManager
{
	public:

	enum class CurrentSong
	{
		outside,
		inside,
		alert,
		evasion,
		caution
	};

	LevelAudioManager();
	~LevelAudioManager();

	void Update(float p_deltaTime);
	void ChangeTrack(CurrentSong p_newSong);
	void ChangeTrack(CurrentSong p_newSong, float p_delay);

	private:


	const char* m_tracksNames[5] =
	{
		"assets/musicOutside.mp3",
		"assets/musicOutside.mp3",
		"assets/musicAlert.mp3",
		"assets/musicEvasion.mp3",
		"assets/musicCaution.mp3",
	};

	Audio::Sound m_tracks[5];
	Audio::Sound* m_ptrCurrentTrack = nullptr;
	float delayTimer = 0;

};

#endif // !LEVEL_AUDIO_MANAGER
