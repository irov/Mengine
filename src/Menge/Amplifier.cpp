#	include "Amplifier.h"

#	include "Playlist.h"

#	include "SoundEngine.h"

#	include "Application.h"

#	include "ResourcePlaylist.h"

#	include "LogEngine.h"

#	include "ResourceManager.h"
#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
	: m_music(0)
	, m_buffer(0)
	, m_currentPlayList(0)
	, m_volume( 1.0f )
	, m_volumeOverride( 1.0f )
	, m_playing( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Amplifier::~Amplifier()
	{
		//_release();
		if(m_music)
		{
			m_music->setSoundNodeListener( 0 );
			Holder<SoundEngine>::hostage()->releaseSoundSource( m_music );
			Holder<SoundEngine>::hostage()->releaseSoundBuffer( m_buffer );
			m_music = 0;
			m_buffer = 0;
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
	bool Amplifier::_loadPlayList( const String& _playlistResource)
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
	void Amplifier::playTrack( const String& _playlistResource, int _index, bool _looped )
	{
		bool enabled = Holder<Application>::hostage()->getSoundEnabled();

		if( !enabled )
		{
			return;
		}

		if(!_loadPlayList(_playlistResource))
		{
			MENGE_LOG << "Amplifier: no found playlist with name " << _playlistResource;
			return;
		}

		m_currentPlayList->setLooped1(_looped);

		const String& name = m_currentPlayList->getTrackByIndex(_index);

		m_currentPlayList->setTrack(name);

		_prepareSound(name);

		m_music->setVolume( m_volume );
		m_music->play();
		m_playing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Amplifier::getNumTracks() const
	{
		if(m_currentPlayList == NULL)
		{
			return 0;
		}

		return m_currentPlayList->numTracks(); 
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::playAllTracks( const String& _playlistResource )
	{
		bool enabled = Holder<Application>::hostage()->getSoundEnabled();

		if( !enabled )
		{
			return;
		}

		if(!_loadPlayList(_playlistResource))
		{
			MENGE_LOG << "Amplifier: no found playlist with name " << _playlistResource;
			return;
		}

		m_currentPlayList->first();

		_prepareSound(m_currentPlayList->getTrack());

		m_music->setVolume( m_volume );
		MENGE_LOG << "Start playing music";
		m_music->play();
		m_playing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::shuffle( const String& _playlist )
	{
		TMapPlayList::iterator it = m_mapPlayLists.find( _playlist );

		if ( it == m_mapPlayLists.end() )
		{			
			ResourcePlaylist * playlistResource = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourcePlaylist>( _playlist );

			if( playlistResource == NULL )
			{
				MENGE_LOG << "Amplifier: no found playlist with name " << _playlist;
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
		m_playing = false;
		if( m_music )
		{
			m_music->stop();
			_release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::listenStopped()
	{
		if( m_playing == false ) return;
		if(m_currentPlayList->getLooped())
		{
			m_currentPlayList->next();
			const String& filename = m_currentPlayList->getTrack();
			_prepareSound(filename);

			if( m_music != 0 )
			{
				m_music->setVolume( m_volume );
				m_music->play();
				m_playing = true;
			}
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
	void Amplifier::_prepareSound( const String& _filename )
	{
		stop();
		//_release();

		m_buffer = Holder<SoundEngine>::hostage()->createSoundBufferFromFile( _filename.c_str(), true );

		if( m_buffer == 0 )
		{
			MENGE_LOG << "Warning: Amplifier can't load sample " << _filename;
			return;			
		}

		m_music = Holder<SoundEngine>::hostage()->createSoundSource( false, m_buffer, this );

		if( m_music == 0 )
		{
			MENGE_LOG << "Warning: Amplifier \"" << _filename << "\" can't create sound source";
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
		if( m_volumeTo.isStarted() )
		{
			float volume;
			bool end = m_volumeTo.update( _timing, &volume );
			setVolume( volume );
			/*if( end == true )
			{
				setVolume( m_volumeOverride );
			}*/
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::volumeTo( float _time, float _volume )
	{
		//m_volumeOverride = m_volume;
		m_volumeTo.start( m_volume, _volume, _time, ::fabsf );
	}
	//////////////////////////////////////////////////////////////////////////
}