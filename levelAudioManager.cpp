#include "precomp.h"
#include "levelAudioManager.h"

LevelAudioManager::LevelAudioManager()
{
	for (int i = 0; i < 5; i++)
	{
		m_tracks[i] = Audio::Sound{ m_tracksNames[i], Audio::Sound::Type::Music};
	}

	// for now default song is outside
	m_ptrCurrentTrack = &m_tracks[static_cast<int>(CurrentSong::outside)];
}

LevelAudioManager::~LevelAudioManager()
{
	m_ptrCurrentTrack = nullptr;
	for (Audio::Sound& track : m_tracks)
	{
		track.stop();
	}
}

void LevelAudioManager::Update(float p_deltaTime)
{
	if (delayTimer > 0)
	{
		delayTimer -= p_deltaTime;
	}
	else if (m_ptrCurrentTrack != nullptr)
	{
		m_ptrCurrentTrack->play();
	}
}

void LevelAudioManager::ChangeTrack(CurrentSong p_newSong)
{
	if (m_ptrCurrentTrack == &m_tracks[static_cast<int>(p_newSong)])
	{
		return;
	}

	m_ptrCurrentTrack->stop();
	if (m_ptrCurrentTrack != &m_tracks[static_cast<int>(p_newSong)])
	{
		m_ptrCurrentTrack = &m_tracks[static_cast<int>(p_newSong)];
		m_ptrCurrentTrack->replay();
	}
}

void LevelAudioManager::ChangeTrack(CurrentSong p_newSong, float p_delay)
{
	if (m_ptrCurrentTrack == &m_tracks[static_cast<int>(p_newSong)])
	{
		return;
	}

	m_ptrCurrentTrack->stop();
	if (m_ptrCurrentTrack != &m_tracks[static_cast<int>(p_newSong)])
	{
		m_ptrCurrentTrack = &m_tracks[static_cast<int>(p_newSong)];
		m_ptrCurrentTrack->replay();
		m_ptrCurrentTrack->stop();
	}
	delayTimer = p_delay;
	//m_ptrCurrentTrack->setStartTime(p_delay);

}

