#	include "Amplifier.h"

#	include "Playlist.h"

#	include "SoundEngine.h"

#	include "ResourcePlaylist.h"

#	include "LogEngine.h"

#	include "ResourceManager.h"
#	include <cmath>

#	include "ScriptEngine.h"

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
	, m_volToCb( NULL )
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
	bool Amplifier::loadPlayList_( const String& _playlistResource )
	{
		TMapPlayList::iterator it = m_mapPlayLists.find( _playlistResource );

		if ( it == m_mapPlayLists.end() )
		{			
			ResourcePlaylist * playlistResource = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourcePlaylist>( _playlistResource );

			if( playlistResource == NULL )
			{
				MENGE_LOG_ERROR( "Amplifier: no found playlist with name '%s'"
					, _playlistResource.c_str()
					);

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
		if( loadPlayList_( _playlistResource ) == false )
		{
			return;
		}

		m_currentPlayList->setLooped1(_looped);

		String name = m_currentPlayList->getTrackByIndex(_index);
		const String& category = m_currentPlayList->getCategory();

		m_currentPlayList->setTrack(_index);

		prepareSound_( category, name );

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
		if(!loadPlayList_(_playlistResource))
		{
			MENGE_LOG_ERROR( "Amplifier: no found playlist with name '%s'"
				, _playlistResource.c_str()
				);

			return;
		}

		m_currentPlayList->first();

		String track = m_currentPlayList->getTrack();
		const String& category = m_currentPlayList->getCategory();

		prepareSound_( category, track );

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
		if( loadPlayList_( _playlist ) == false )
		{
			return;
		}

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
			release_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::listenStopped()
	{
		if( m_playing == false ) return;
		if(m_currentPlayList->getLooped())
		{
			m_currentPlayList->next();
			String filename = m_currentPlayList->getTrack();
			const String& category = m_currentPlayList->getCategory();
			prepareSound_( category, filename );

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
	void Amplifier::prepareSound_( const String& _pakName, const String& _filename )
	{
		stop();
		//_release();

		m_buffer = Holder<SoundEngine>::hostage()
						->createSoundBufferFromFile( _pakName, _filename, true );

		if( m_buffer == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Amplifier can't load sample '%s'"
				, _filename.c_str() 
				);

			return;			
		}

		m_sourceID = Holder<SoundEngine>::hostage()->createSoundSource( false, m_buffer, true );

		if( m_sourceID == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Amplifier '%s' can't create sound source"
				, _filename.c_str()
				);

			return;
		}
		Holder<SoundEngine>::hostage()
			->setSourceListener( m_sourceID, this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::release_()
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
		/*if( m_volumeTo.isStarted() )
		{
			float volume;
			bool end = m_volumeTo.update( _timing, &volume );
			Holder<SoundEngine>::hostage()
				->setMusicVolume( volume );
		}*/
		float value;
		bool finish = false;
		//if( m_endFlag == true )
		if( m_volumeTo.isStarted() == true )
		{
			finish = m_volumeTo.update( _timing, &value );
			Holder<SoundEngine>::hostage()
				->setMusicVolume( value );
		}
		if( finish == true )
		{
			if( m_volToCb != NULL
				&& m_volToCb != Py_None )
			{
				PyObject* cb = m_volToCb;
				pybind::call( cb, "()" );
				pybind::decref( cb );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::volumeTo( float _time, float _volume )
	{
		//m_volumeOverride = m_volume;
		m_volToCb = NULL;
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
	void Amplifier::volumeToCb( float _time, float _value, PyObject* _cb )
	{
		m_volToCb = _cb;
		pybind::incref( m_volToCb );
		float volume = Holder<SoundEngine>::hostage()->getMusicVolume();
		m_volumeTo.start( volume, _value, _time, ::fabsf );
	}
	//////////////////////////////////////////////////////////////////////////
}
