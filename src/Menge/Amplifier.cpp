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
	, m_playlistResource(0)
	, m_volume( 1.0f )
	, m_changeTrack( false )
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
		release();

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

			const std::string & playlistName = m_playlistResource->getName();
			const std::vector<std::string> & tracks = m_playlistResource->getTracks();
			bool is_looped = m_playlistResource->getLooped();

			Playlist * playlist = new Playlist( playlistName );

			for( std::vector<std::string>::const_iterator i = tracks.begin(); i != tracks.end(); ++i )
			{
				playlist->addTrack( *i );
			}

			playlist->setLooped( is_looped );

			it = m_playLists.insert(
				std::make_pair( playlistName, playlist )
				).first;
		}

		m_currentPlayList = it->second;
		m_currentPlayList->firstSong();

		m_changeTrack = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::stop()
	{
		if( m_music )
		{
			m_music->stop();
			release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::listenStopped()
	{
		if ( m_currentPlayList->isEnded() && m_currentPlayList->getLooped() == false )
		{
			m_changeTrack = false;
			return;
		}
		m_changeTrack = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::listenPaused( bool _pause )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::setVolume( float _vol )
	{
		if ( _vol >= 1 ) _vol = 1.0f;
		if ( _vol <= 0 ) _vol = 0.0f;

		m_volume = _vol * Holder<SoundEngine>::hostage()->getCommonVolume();

		m_music->setVolume( _vol );
	}
	//////////////////////////////////////////////////////////////////////////
	float	Amplifier::getVolume() const
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::initStream()
	{
		const std::string & filename = m_currentPlayList->getCurrentSongName();

		m_sampleMusic = Holder<SoundEngine>::hostage()->createSoundBuffer();
		m_sampleMusic->loadFromFile( filename.c_str(), true );

		m_music = Holder<SoundEngine>::hostage()->createSoundSource( false, m_sampleMusic, this );
		m_music->play();
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::release()
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource( m_music);
		Holder<SoundEngine>::hostage()->releaseSoundBuffer( m_sampleMusic );
		m_music = NULL;
		m_sampleMusic = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::update( size_t _timing )
	{
		if( m_changeTrack == true ) //squall bug :(
		{
			release();
			initStream();
			m_currentPlayList->nextSong();
			m_changeTrack = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}