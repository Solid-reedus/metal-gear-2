#include "precomp.h"
#include "enemySoundManager.h"

EnemySoundManager::EnemySoundManager()
{
	for (int i = 0; i < 2; i++)
	{
		m_sfx[i] = Audio::Sound{ m_sfxNames[i], Audio::Sound::Type::Sound };
		m_sfx[i].setLooping(false);
	}

}

void EnemySoundManager::Play(EnemySound p_sound)
{
	if (m_currentSfx != nullptr)
	{
		m_currentSfx->stop();
	}
	m_currentSfx = &m_sfx[static_cast<int>(p_sound)];
	m_currentSfx->play();


}
