#	include "BackgroundSound.h"
#	include "FileEngine.h"
#	include "SoundSystemInterface.h"

namespace	Menge
{
	BackgroundSound::BackgroundSound()
	: m_soundSource(0)
	, m_fadeVelocity(0.003f)
	{
		m_fadeState = NO_FADE;
	}

	void	BackgroundSound::_beginFade()
	{
		m_fadeState = FADE_UP;
		m_currentSoundTrackName = m_currentPlayList->getCurrentSongName();
		Keeper<SoundEngine>::hostage()->addSoundNode(m_soundSource,m_fileData,m_currentSoundTrackName,this,true);
		m_soundSource->setVolume(0);
		m_soundSource->play();
	}

	void	BackgroundSound::play(Playlist& _playList)
	{
		m_currentPlayList = &_playList;

		static bool firstInit = true;

		if (firstInit)
		{
			_beginFade();
			firstInit = false;
			return;
		}

		if (m_soundSource->isPlaying() && !firstInit)
		{
			m_fadeState = FADE_DOWN;
		}
		else
		{
			_beginFade();
		}
	}

	bool	BackgroundSound::listenRecycled(SoundSourceInterface* _sn)
	{
		printf("listenRecycled \n");
		return true;
	}

	void	BackgroundSound::listenStoped(SoundSourceInterface*	_sn)
	{
		printf("listenStoped \n");
	} 

	void	BackgroundSound::listenEnded(SoundSourceInterface* _sn)
	{
		m_currentPlayList->nextSong();

		Keeper<SoundEngine>::hostage()->deleteSound(m_soundSource);
		Keeper<FileEngine>::hostage()->closeFile(m_fileData);

		_beginFade();

		printf("listenEnded \n");
	}

	void	BackgroundSound::update()
	{
		m_soundSource->updateSoundBuffer();

		switch(m_fadeState)
		{
			case FADE_UP:
			{
				printf("FADE UP \n");
				float newVolume = m_soundSource->getVolume() + m_fadeVelocity;
				m_soundSource->setVolume(newVolume);

				if (m_soundSource->getVolume() >= 1.0f)
				{
					m_soundSource->setVolume(1.0f);
					m_fadeState = NO_FADE;
				}
			}
			break;

			case FADE_DOWN:
			{
				printf("FADE_DOWN \n");
				float newVolume = m_soundSource->getVolume() - m_fadeVelocity;
				m_soundSource->setVolume(newVolume);

				if (m_soundSource->getVolume() <= 0)
				{
					m_soundSource->stop();
				}
			}
			break;

			case NO_FADE:
			{
				printf("NO_FADE \n");

				double totalSize = m_soundSource->getTotalSize() * 0.50;
				double currPos = m_soundSource->getPos();
			
				if (currPos >= totalSize)	

				//if ((totalSize - currPos) <= 5.0)	
				{
					m_fadeState = FADE_DOWN;
				}
			}
			break;

			default:
			{
				assert(!"undefined state!");
			}
			break;
		};
	};
};