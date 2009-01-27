#	include "Amplifier.h"

#	include "Playlist.h"

#	include "SoundEngine.h"

#	include "Application.h"

#	include "ResourcePlaylist.h"

#	include "LogEngine.h"

#	include "ResourceManager.h"
#	include <cmath>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
	: m_sourceID( 0 )
	, m_buffer( NULL )
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
		if( m_sourceID != 0 )
		{
			Holder<SoundEngine>::hostage()
				->releaseSoundSource( m_sourceID );
			Holder<SoundEngine>::hostage()
				->releaseSoundBuffer( m_buffer );
			m_sourceID = 0;
			m_buffer = NULL;
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
			MENGE_LOG_ERROR( "Amplifier: no found playlist with name \"%s\""
				, _playlistResource.c_str() );
			return;
		}

		m_currentPlayList->setLooped1(_looped);

		const String& name = m_currentPlayList->getTrackByIndex(_index);

		m_currentPlayList->setTrack(_index);

		_prepareSound(name);

		Holder<SoundEngine>::hostage()
			->setVolume( m_sourceID, Holder<SoundEngine>::hostage()->getMusicVolume() );
		Holder<SoundEngine>::hostage()
			->play( m_sourceID );
		
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
			MENGE_LOG_ERROR( "Amplifier: no found playlist with name \"%s\""
				, _playlistResource.c_str() );
			return;
		}

		m_currentPlayList->first();

		const String & track = m_currentPlayList->getTrack();

		_prepareSound(track);

		if( m_sourceID != 0 )
		{
			//Holder<SoundEngine>::hostage()
			//	->setVolume( m_sourceID, Holder<SoundEngine>::hostage()->getMusicVolume() );
			Holder<SoundEngine>::hostage()
				->play( m_sourceID );
		}
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
				MENGE_LOG_ERROR( "Amplifier: no found playlist with name \"%s\""
					, _playlist.c_str() );
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
		if( m_sourceID != 0 )
		{
			Holder<SoundEngine>::hostage()
				->stop( m_sourceID );
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

			if( m_sourceID != 0 )
			{
				//Holder<SoundEngine>::hostage()
				//	->setVolume( m_sourceID, Holder<SoundEngine>::hostage()->getMusicVolume() );
				Holder<SoundEngine>::hostage()
					->play( m_sourceID );
				m_playing = true;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::listenPaused()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::_prepareSound( const String& _filename )
	{
		stop();
		//_release();

		m_buffer = Holder<SoundEngine>::hostage()->createSoundBufferFromFile( _filename.c_str(), true );

		if( m_buffer == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Amplifier can't load sample \"%s\""
				, _filename.c_str() );
			return;			
		}

		m_sourceID = Holder<SoundEngine>::hostage()->createSoundSource( false, m_buffer, true );

		if( m_sourceID == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Amplifier \"%s\" can't create sound source"
				, _filename.c_str() );
			return;
		}
		Holder<SoundEngine>::hostage()
			->setSourceListener( m_sourceID, this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::_release()
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource( m_sourceID );
		Holder<SoundEngine>::hostage()->releaseSoundBuffer( m_buffer );
		m_sourceID = 0;
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
			Holder<SoundEngine>::hostage()
				->setMusicVolume( volume );
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
		float volume = Holder<SoundEngine>::hostage()->getMusicVolume();
		m_volumeTo.start( volume, _volume, _time, ::fabsf );
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getPosMs()
	{
		if( m_sourceID == 0 )
		{
			return 0.0f;
		}
		return Holder<SoundEngine>::hostage()
			->getPosMs( m_sourceID );
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::setPosMs( float _posMs )
	{
		if( m_sourceID != 0 )
		{
			Holder<SoundEngine>::hostage()
				->setPosMs( m_sourceID, _posMs );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
