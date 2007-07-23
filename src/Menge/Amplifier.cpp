#	include "Amplifier.h"

#	include "Playlist.h"

#	include "XmlParser/XmlParser.h"

#	include "FileEngine.h"

#	include "SoundEngine.h"

namespace Menge
{

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
		Holder<Amplifier>::keep(this);
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
	void	Amplifier::updateFadeParams(SoundSourceInterface* _sound)
	{
		float ticks = (float)clock();

		float time_offset_begin = (ticks - m_timeFadeBegin);

		if(m_fadeState == false)
		{
			if (_sound->getVolume() >= 1.0f)
			{
				_sound->setVolume(1.0f);
				m_timeFadeEnd = clock();
				m_fadeState = true;
			}
			else
			{
				float new_volume = time_offset_begin / m_fadeTime;
				_sound->setVolume(new_volume);
				printf("%f \n",new_volume);
			}
		}
		else
		{
			if(_sound->getVolume() > 0)
			{
				if(time_offset_begin >= m_fadeoffTime)
				{
					float time_offset_end = (ticks - m_timeFadeEnd);
					float new_volume = (m_fadeoffTime - time_offset_end) / m_fadeTime;
					_sound->setVolume(new_volume);
					printf("%f \n",new_volume);
				}
			}
		}      
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::update(float _timing)
	{
		if(m_isPaused)
		{
			return;
		}

		if(m_music)
		{
			updateFadeParams(m_music);
		}

		if(m_isMusicDead)
		{
			releaseMusic(false);
		
			std::string filename = m_currentPlayList->getCurrentSongName();

			m_sampleMusic = Holder<SoundEngine>::hostage()->createSoundBuffer();
			m_sampleMusic->loadFromFile(filename.c_str(),true);

			m_music = Holder<SoundEngine>::hostage()->createSoundSource(false,m_sampleMusic,this);
			m_music->play();
			m_music->setVolume(0);
			// 				
			m_currentPlayList->nextSong();

			beginFade();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::beginFade()
	{
		assert(m_music->getLengthMS() >= 2 * m_fadeTime);
		m_fadeoffTime = m_music->getLengthMS() - m_fadeTime;
		m_timeFadeBegin = clock();
		m_fadeState = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::releaseMusic(bool _dead)
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource(m_music);
		m_music = NULL;

		Holder<SoundEngine>::hostage()->releaseSoundBuffer(m_sampleMusic);
		m_sampleMusic = NULL;
		m_isMusicDead = _dead;
	}
	//////////////////////////////////////////////////////////////////////////
}