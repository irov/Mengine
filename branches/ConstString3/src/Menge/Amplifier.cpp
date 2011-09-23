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
		this->stop();

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
	bool Amplifier::loadPlayList_( const ConstString& _playlistResource )
	{
		TMapPlayList::iterator it = m_mapPlayLists.find( _playlistResource );

		if ( it == m_mapPlayLists.end() )
		{			
			ResourcePlaylist * playlistResource = ResourceManager::get()
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
	void Amplifier::playTrack( const ConstString& _playlistResource, int _index, bool _looped )
	{
		if( loadPlayList_( _playlistResource ) == false )
		{
			return;
		}

		m_currentPlayList->setLooped1(_looped);

		const TrackDesc * desc = m_currentPlayList->getTrackByIndex(_index);
		const ConstString & category = m_currentPlayList->getCategory();

		m_currentPlayList->setTrack(_index);

		prepareSound_( category, desc->path, desc->codec );

		float musicVolume = 
			SoundEngine::get()->getMusicVolume();

		SoundEngine::get()
			->setSourceVolume( m_sourceID, musicVolume );

		SoundEngine::get()
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
	void Amplifier::playAllTracks( const ConstString& _playlistResource )
	{
		if( this->loadPlayList_(_playlistResource) == false )
		{
			MENGE_LOG_ERROR( "Amplifier: no found playlist with name '%s'"
				, _playlistResource.c_str()
				);

			return;
		}

		m_currentPlayList->first();

		const TrackDesc * track = m_currentPlayList->getTrack();

		if( track == 0 )
		{
			return;
		}
		
		const ConstString & category = m_currentPlayList->getCategory();
		this->prepareSound_( category, track->path, track->codec );
	
		if( m_sourceID == 0 )
		{
			return;
		}
		//Holder<SoundEngine>::get()
		//	->setVolume( m_sourceID, Holder<SoundEngine>::get()->getMusicVolume() );

		SoundEngine::get()
			->play( m_sourceID );

		m_playing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::shuffle( const ConstString & _playlist )
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
			SoundEngine::get()
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
			const TrackDesc * track = m_currentPlayList->getTrack();

			if( track )
			{
				const ConstString & category = m_currentPlayList->getCategory();
				prepareSound_( category, track->path, track->codec );
			}

			if( m_sourceID != 0 )
			{
				//Holder<SoundEngine>::get()
				//	->setVolume( m_sourceID, Holder<SoundEngine>::get()->getMusicVolume() );
				SoundEngine::get()
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
	void Amplifier::prepareSound_( const ConstString& _pakName, const ConstString& _file, const ConstString& _codec )
	{
		stop();
		//_release();

		m_buffer = SoundEngine::get()
			->createSoundBufferFromFile( _pakName, _file, _codec, true );

		if( m_buffer == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Amplifier can't load sample '%s'"
				, _file.c_str() 
				);

			return;			
		}

		m_sourceID = SoundEngine::get()
			->createSoundSource( false, m_buffer, true );

		if( m_sourceID == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Amplifier '%s' can't create sound source"
				, _file.c_str()
				);

			return;
		}
		
		SoundEngine::get()
			->setSourceListener( m_sourceID, this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::release_()
	{
		SoundEngine::get()
			->releaseSoundSource( m_sourceID );

		SoundEngine::get()
			->releaseSoundBuffer( m_buffer );

		m_sourceID = 0;
		m_buffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& Amplifier::getPlaying() const
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
			SoundEngine::get()
				->setMusicVolume( volume );
		}*/
		float value;
		bool finish = false;

		if( m_volumeTo.isStarted() == true )
		{
			finish = m_volumeTo.update( _timing, &value );

			SoundEngine::get()
				->setMusicVolume( value );
		}
		if( finish == true )
		{
			if( m_volToCb != NULL && pybind::is_none(m_volToCb) == false )
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

		float volume = SoundEngine::get()
			->getMusicVolume();

		m_volumeTo.start( volume, _volume, _time, ::fabsf );
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getPosMs()
	{
		if( m_sourceID == 0 )
		{
			return 0.0f;
		}
		return SoundEngine::get()
			->getPosMs( m_sourceID );
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::setPosMs( float _posMs )
	{
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->setPosMs( m_sourceID, _posMs );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::volumeToCb( float _time, float _value, PyObject* _cb )
	{
		m_volToCb = _cb;
		pybind::incref( m_volToCb );

		float volume = SoundEngine::get()
			->getMusicVolume();

		m_volumeTo.start( volume, _value, _time, ::fabsf );
	}
	//////////////////////////////////////////////////////////////////////////
}
