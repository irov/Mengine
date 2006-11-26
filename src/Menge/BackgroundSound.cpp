#	include "BackgroundSound.h"
#	include "SoundSystemInterface.h"

namespace	Menge
{
	BackgroundSound::BackgroundSound()
	{
		m_fadeState = NO_FADE;
	}

	void	BackgroundSound::_beginFade()
	{
		m_fadeState = FADE_UP;
		m_currentSoundTrackName = m_currentPlayList->getCurrentSongName();
		Keeper<SoundEngine>::hostage()->addSoundNode(&m_playingTrack,m_currentSoundTrackName,this,true);
		m_playingTrack.play();
		m_playingTrack.setVolume(0);
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

		if (m_playingTrack.isPlaying() && !firstInit)
		{
			m_fadeState = FADE_DOWN;
		}
		else
		{
			_beginFade();
		}
	}

	bool	BackgroundSound::listenRecycled(SoundSourceInterface*	_sn)
	{
		printf("listenRecycled \n");
		return true;
	}

	void	BackgroundSound::listenStoped(SoundSourceInterface*	_sn)
	{
		printf("listenStoped \n");
	} 

	void	BackgroundSound::listenEnded(SoundSourceInterface*	_sn)
	{
		m_currentPlayList->nextSong();

		_beginFade();

		printf("listenEnded \n");
	}

	void	BackgroundSound::update()
	{
		switch(m_fadeState)
		{
			case FADE_UP:
			{
				printf("FADE UP \n");
				float newVolume = m_playingTrack.getVolume()+0.005f;
				m_playingTrack.setVolume(newVolume);

				if (m_playingTrack.getVolume() >= 1.0f)
				{
					m_playingTrack.setVolume(1.0f);
					m_fadeState = NO_FADE;
				}
			}
			break;

			case FADE_DOWN:
			{
				printf("FADE_DOWN \n");
				float newVolume = m_playingTrack.getVolume()-0.005f;
				m_playingTrack.setVolume(newVolume);

				if (m_playingTrack.getVolume() <= 0)
				{
					m_playingTrack.stop();
				}
			}
			break;

			case NO_FADE:
			{
				printf("NO_FADE \n");
				double a = m_playingTrack.getTotalSize();
				double b = m_playingTrack.getPos();
			
				if ((a - b) <= 1.0)	
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