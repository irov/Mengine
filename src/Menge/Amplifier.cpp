#	include "Amplifier.h"

#	include "Playlist.h"

#	include "SoundEngine.h"

#	include "Application.h"

#	include "ResourcePlaylist.h"

#	include "LogEngine.h"

#	include "ResourceManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
	: m_music(0)
	, m_buffer(0)
	, m_currentPlayList(0)
	, m_volume( 1.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Amplifier::~Amplifier()
	{
		if(m_music)
		{
			m_music->setSoundNodeListener(NULL);
		}

		for( TMapPlayList::iterator 
			it = m_mapPlayLists.begin(),
			it_end = m_mapPlayLists.end();
		it != it_end;
		++it)
		{
			delete it->second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::_loadPlayList(const std::string & _playlistResource)
	{
		TMapPlayList::iterator it = m_mapPlayLists.find( _playlistResource );

		if ( it == m_mapPlayLists.end() )
		{			
			ResourcePlaylist * playlistResource = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourcePlaylist>( _playlistResource );

			if( playlistResource == NULL )
			{
				return false;
			}

			Playlist * playlist = new Playlist( playlistResource );

			it = m_mapPlayLists.insert( std::make_pair( _playlistResource, playlist ) ).first;
		}

		m_currentPlaylistName = _playlistResource;
		m_currentPlayList = it->second;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::playTrack( const std::string & _playlistResource, int _index, bool _looped )
	{
		bool enabled = Holder<Application>::hostage()->getSoundEnabled();

		if( !enabled )
		{
			return;
		}

		if(!_loadPlayList(_playlistResource))
		{
			MENGE_LOG("Amplifier: no found playlist with name %s \n", _playlistResource.c_str() );
			return;
		}

		m_currentPlayList->setLooped1(_looped);

		const std::string & name = m_currentPlayList->getTrackByIndex(_index);

		m_currentPlayList->setTrack(name);

		_prepareSound(name);

		m_music->setVolume( m_volume );
		m_music->play();
	}
	//////////////////////////////////////////////////////////////////////////
	int Amplifier::getNumTracks() const
	{
		if(m_currentPlayList == NULL)
		{
			return 0;
		}

		return m_currentPlayList->numTracks(); 
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::playAllTracks( const std::string & _playlistResource )
	{
		bool enabled = Holder<Application>::hostage()->getSoundEnabled();

		if( !enabled )
		{
			return;
		}

		if(!_loadPlayList(_playlistResource))
		{
			MENGE_LOG("Amplifier: no found playlist with name %s \n", _playlistResource.c_str() );
			return;
		}

		m_currentPlayList->first();

		_prepareSound(m_currentPlayList->getTrack());

		m_music->setVolume( m_volume );
		m_music->play();
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::shuffle( const std::string & _playlist )
	{
		TMapPlayList::iterator it = m_mapPlayLists.find( _playlist );

		if ( it == m_mapPlayLists.end() )
		{			
			ResourcePlaylist * playlistResource = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourcePlaylist>( _playlist );

			if( playlistResource == NULL )
			{
				MENGE_LOG("Amplifier: no found playlist with name %s \n", _playlist.c_str() );
				return;
			}

			Playlist * playlist = new Playlist( playlistResource );

			it = m_mapPlayLists.insert( std::make_pair( _playlist, playlist ) ).first;
		}

		m_currentPlayList = it->second;
		m_currentPlayList->shuffle();
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::stop()
	{
		if( m_music )
		{
			m_music->stop();
			_release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::listenStopped()
	{
		if(m_currentPlayList->getLooped())
		{
			m_currentPlayList->next();
			const std::string & filename = m_currentPlayList->getTrack();
			_prepareSound(filename);

			m_music->setVolume( m_volume );
			m_music->play();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::listenPaused( bool _pause )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::setVolume( float _volume )
	{
		float commonValue = Holder<SoundEngine>::hostage()->getCommonVolume();
		m_volume = _volume * commonValue;

		if( m_music )
		{
			m_music->setVolume( m_volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getVolume() const
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::_prepareSound(const std::string & _filename)
	{
		_release();

		m_buffer = Holder<SoundEngine>::hostage()->createSoundBufferFromFile( _filename.c_str(), true );

		if( m_buffer == 0 )
		{
			MENGE_LOG("Warning: Amplifier can't load sample '%s'\n"
				, _filename.c_str() 
				);

			return;			
		}

		m_music = Holder<SoundEngine>::hostage()->createSoundSource( false, m_buffer, this );

		if( m_music == 0 )
		{
			MENGE_LOG("Warning: Amplifier '%s' can't create sound source\n"
				, _filename.c_str()
				);

			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::_release()
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource( m_music );
		Holder<SoundEngine>::hostage()->releaseSoundBuffer( m_buffer );
		m_music = NULL;
		m_buffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Amplifier::getPlaying() const
	{
		return m_currentPlaylistName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::update( float _timing )
	{
		if( m_fade.isStarted() )
		{
			float volume;
			m_fade.update( _timing, &volume );
			setVolume( volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::fadeIn( float _time )
	{
		m_fade.start( 0.0f, m_volume, _time, ::fabsf );
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::fadeOut( float _time )
	{
		m_fade.start( m_volume, 0.0f, _time, ::fabsf );
	}
	//////////////////////////////////////////////////////////////////////////
}