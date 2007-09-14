#	include "Amplifier.h"

#	include "Playlist.h"

#	include "XmlParser/XmlParser.h"

#	include "FileEngine.h"

#	include "SoundEngine.h"

//#	include "TimeInterval.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
	: m_music(0)
	, m_sampleMusic(0)
	, m_currentPlayList(0)
	, m_fadeState(false)
	, m_isMusicDead(false)
	, m_isPaused(false)
	, m_fadeTime(3000.0f) //3000 ms
	{
		assert(m_fadeTime > 0);
	}
	//////////////////////////////////////////////////////////////////////////
	Amplifier::~Amplifier()
	{
		for each( const TPlayListMap::value_type & it in m_playLists )
		{
			delete it.second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::playList(const std::string& _playListName)
	{
		TPlayListMap::iterator it = m_playLists.find(_playListName);

		if (it == m_playLists.end())
		{
			assert(!"PlayList not found!");
			return;
		}

		m_currentPlayList = it->second;
		m_currentPlayList->loadTracks();

		releaseMusic(true);
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::stop()
	{
		if(m_music)
		{
			m_music->stop();
			releaseMusic(false);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::addPlayList(const std::string& _playListFileName)
	{
		m_playLists.insert(
			std::make_pair(_playListFileName, new Playlist(_playListFileName))
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::erasePlayList(const std::string& _playListFileName)
	{
		m_playLists.erase(_playListFileName);
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::loadPlayList(const std::string& _filename)
	{
		std::string playListFileName;

		TiXmlDocument * document = Holder<FileEngine>::hostage()
			->loadXml( _filename );

		XML_FOR_EACH_DOCUMENT( document )
		{
			XML_CHECK_NODE_FOR_EACH("Pls")
			{
				XML_CHECK_VALUE_NODE("Pl","File",playListFileName)
				{
					addPlayList(playListFileName);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::listenPaused(bool _pause)
	{
		m_isPaused = _pause;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::listenStopped()
	{
		m_isMusicDead = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::setVolume(float _newVolume)
	{
		#ifdef _DEBUG
	//		printf("%f \n",_newVolume);
		#endif

		if(_newVolume >= 1) _newVolume = 1.0f;
		if(_newVolume <= 0) _newVolume = 0.0f;

		m_music->setVolume(_newVolume);
	}
	
	//////////////////////////////////////////////////////////////////////////

	//TimeInterval timer(1000, true, true, true);

	void	Amplifier::updateFadeParams(SoundSourceInterface* _sound)
	{
		if(_sound == NULL) return;

	//	static int position = 0;
	//	position++;

	/*	int  position = _sound->getPosMs();

		int beginFading = m_music->getLengthMS() - (int)m_fadeTime;

		if(timer.value() != 1.0f || position > beginFading)
		{
			float value = 0.0f;

			if( position > beginFading)
				value = timer.update( position - beginFading );
			else
				value = timer.update( position );

			if(value == 0.0f)
			{
				int u = 0;
			}

				
			setVolume(value);
			printf("%f \n",value);
		}
*/
	
		/*if( position > beginFading)
		{
			float value = timer.update( position - beginFading );
			setVolume(value);
			printf("%f \n",value);
		}*/

		if(_sound == NULL) return;

		int  position = _sound->getPosMs();

		//printf("%d \n", position);

		if(m_fadeState == false)
		{
			if(position >= m_fadeTime)
			{
				setVolume(1.0f);
				m_fadeState = true;
			}
			else
			{
				float new_volume = position / m_fadeTime;
				setVolume(new_volume);
			}
		}
		else
		{
			int len = m_music->getLengthMS();

			int beginFading = len - (int)m_fadeTime;

			if(position >= beginFading)
			{
				float new_volume = (len - position) / m_fadeTime;
				setVolume(new_volume);
			}
		}     
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::update( float _timing )
	{
		if ( m_isPaused )
		{
			return;
		}

		if ( m_isMusicDead )
		{
			printf("music dead = true \n");
			releaseMusic(false);
		
			const std::string & filename = m_currentPlayList->getCurrentSongName();

			m_sampleMusic = Holder<SoundEngine>::hostage()->createSoundBuffer();
			m_sampleMusic->loadFromFile( filename.c_str(), true );

			m_music = Holder<SoundEngine>::hostage()->createSoundSource( false, m_sampleMusic, this );

			m_music->play();
			m_music->setVolume( 0.0f );
			
			m_currentPlayList->nextSong();

			m_fadeState = false;

			int length = m_music->getLengthMS();

			assert( length >= 2 * m_fadeTime );
		
			//timer.start( m_fadeTime, length, true, false );
		}

	/*	if(m_music)
		{

		int  position = m_music->getPosMs();

		printf("%d \n", position);
		}
		*/
		updateFadeParams(m_music);
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::releaseMusic( bool _dead )
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource(m_music);
		m_music = NULL;

		Holder<SoundEngine>::hostage()->releaseSoundBuffer(m_sampleMusic);
		m_sampleMusic = NULL;
		m_isMusicDead = _dead;
	}
	//////////////////////////////////////////////////////////////////////////
}