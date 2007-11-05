#	include "Amplifier.h"

#	include "Playlist.h"

#	include "XmlParser/XmlParser.h"

#	include "FileEngine.h"

#	include "SoundEngine.h"

#	include "ResourcePlaylist.h"

#	include "ResourceManager.h"

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
	, m_playlistResource(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	Amplifier::~Amplifier()
	{
		if( m_playlistResource )
		{
			Holder<ResourceManager>::hostage()
				->releaseResource( m_playlistResource );
		}

		for each( const TPlayListMap::value_type & it in m_playLists )
		{
			delete it.second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::playList( const std::string & _playListName )
	{
		TPlayListMap::iterator it = m_playLists.find( _playListName );

		if ( it == m_playLists.end() )
		{
			if( m_playlistResource )
			{
				Holder<ResourceManager>::hostage()
					->releaseResource( m_playlistResource );
			}

			m_playlistResource = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourcePlaylist>( _playListName );

			addPlayList( m_playlistResource->getPlayListName()
				, m_playlistResource->getTracks() );

			it = m_playLists.find( _playListName );
		}

		m_currentPlayList = it->second;
		m_currentPlayList->firstSong();

		releaseMusic( true );
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::stop()
	{
		if( m_music )
		{
			m_music->stop();
			releaseMusic( false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::addPlayList( const std::string& _playListFileName, const std::vector<std::string> & _tracks )
	{
		Playlist * playlist = new Playlist( _playListFileName );

		for( std::vector<std::string>::const_iterator it = _tracks.begin(); it != _tracks.end(); ++it )
		{
			playlist->addTrack( *it );
		}

		m_playLists.insert(
			std::make_pair( _playListFileName, playlist )
			);
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
		if(_newVolume >= 1) _newVolume = 1.0f;
		if(_newVolume <= 0) _newVolume = 0.0f;

		m_music->setVolume(_newVolume);
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::update( size_t _timing )
	{
		if ( m_isPaused )
		{
			return;
		}

		if ( m_isMusicDead )
		{
			releaseMusic( false );
		
			const std::string & filename = m_currentPlayList->getCurrentSongName();

			m_sampleMusic = Holder<SoundEngine>::hostage()->createSoundBuffer();
			m_sampleMusic->loadFromFile( filename.c_str(), true );

			m_music = Holder<SoundEngine>::hostage()->createSoundSource( false, m_sampleMusic, this );

			m_music->play();

			m_currentPlayList->nextSong();

			#ifdef	FADE_PARAM
				m_music->setVolume( 0.0f );
				m_fadeState = false;
				int length = m_music->getLengthMS();
				assert( length >= 2 * m_fadeTime );
			#endif
		}

		#ifdef	FADE_PARAM
			if( m_music )
			{
				updateFadeParams(m_music);
			}
		#endif
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::releaseMusic( bool _dead )
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource( m_music);
		m_music = NULL;

		Holder<SoundEngine>::hostage()->releaseSoundBuffer( m_sampleMusic );
		m_sampleMusic = NULL;
		m_isMusicDead = _dead;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::updateFadeParams(SoundSourceInterface* _sound)
	{
		if( _sound == NULL ) return;

		int  position = _sound->getPosMs();

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

			if( position >= beginFading )
			{
				float new_volume = (len - position) / m_fadeTime;
				setVolume(new_volume);
			}
		}     
	}
	//////////////////////////////////////////////////////////////////////////
}