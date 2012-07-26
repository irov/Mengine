#	include "AmplifierService.h"

#	include "Playlist.h"

//#	include "SoundEngine.h"

#	include "ResourcePlaylist.h"

#	include "LogEngine.h"

#	include "ResourceManager.h"
#	include <cmath>

#	include "ScriptEngine.h"
#	include "Consts.h"

bool initializeAmplifierService( Menge::AmplifierServiceInterface ** _service )
{
	if( _service == NULL )
	{
		return false;
	}

	*_service = new Menge::AmplifierService;

	return true;
}

void finalizeAmplifierService( Menge::AmplifierServiceInterface * _service )
{
	delete _service;
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AmplifierService::AmplifierService()
		: m_sourceID(0)
		, m_buffer(NULL)
		, m_currentPlayList(0)
		, m_volume(1.0f)
		, m_volumeOverride(1.0f)
		, m_playing(false)
		, m_currentSoundPosition(0.0f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AmplifierService::~AmplifierService()
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
	bool AmplifierService::loadPlayList_( const ConstString& _playlistResource )
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
	void AmplifierService::resetPlayList()
	{	
		m_currentPlayList = NULL;

		m_currentPlaylistName = 
			Consts::get()->c_builtin_empty;

		if( m_sourceID != 0 )
		{
			Holder<SoundServiceInterface>::get()
				->stop( m_sourceID );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::playTrack( const ConstString& _playlistResource, int _index, bool _looped )
	{
		if( loadPlayList_( _playlistResource ) == false )
		{
			return;
		}

		m_currentPlayList->setLooped1(_looped);

		const TrackDesc * desc = m_currentPlayList->getTrackByIndex(_index);
		const ConstString & category = m_currentPlayList->getCategory();

		m_currentPlayList->setTrack(_index);

		this->prepareSound_( category, desc->path, desc->codec );

		float musicVolume = Holder<SoundServiceInterface>::get()
			->getMusicVolume();

		if( Holder<SoundServiceInterface>::get()
			->setSourceVolume( m_sourceID, musicVolume ) == false )
		{
			MENGE_LOG_ERROR("AmplifierService::playTrack invalid set source volume %S"
				, desc->path.c_str()
				);
		}

		Holder<SoundServiceInterface>::get()
			->play( m_sourceID );
		
		m_playing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t AmplifierService::getNumTracks() const
	{
		if(m_currentPlayList == NULL)
		{
			return 0;
		}

		return m_currentPlayList->numTracks();
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::playAllTracks( const ConstString& _playlistResource )
	{
		if( this->loadPlayList_(_playlistResource) == false )
		{
			MENGE_LOG_ERROR( "AmplifierService: no found playlist with name '%s'"
				, _playlistResource.c_str()
				);

			return;
		}

		m_currentPlayList->first();

		this->play();
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::play()
	{
		
		//Holder<SoundEngine>::get()
		//	->setVolume( m_sourceID, Holder<SoundEngine>::get()->getMusicVolume() );
		if( this->preparePlay_() == false )
		{
			return;
		}

		this->play_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool AmplifierService::preparePlay_()
	{
		if( m_currentPlayList == NULL )
		{
			return false;
		}

		const TrackDesc * track = m_currentPlayList->getTrack();

		if( track == 0 )
		{
			return false;
		}

		const ConstString & category = m_currentPlayList->getCategory();
		this->prepareSound_( category, track->path, track->codec );

		if( m_sourceID == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::play_()
	{
		Holder<SoundServiceInterface>::get()
			->play( m_sourceID );

		m_playing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::shuffle( const ConstString & _playlist )
	{
		if( loadPlayList_( _playlist ) == false )
		{
			return;
		}

		m_currentPlayList->shuffle();
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::stop()
	{
		m_playing = false;

		if( m_sourceID != 0 )
		{
			Holder<SoundServiceInterface>::get()
				->stop( m_sourceID );

			release_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::pause()
	{
		if( m_sourceID == 0 )
		{
			return;
		}

		m_playing = false;

		Holder<SoundServiceInterface>::get()
			->pause( m_sourceID );
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::onTurnSound( bool _turn )
	{
		if( _turn == false )
		{
			if( m_sourceID == 0)
			{
				return;
			}

			m_currentSoundPosition = this->getPosMs();
			
			this->stop();
		}
		else
		{
			//this->playMs_( m_currentSoundPosition );			
			this->play();

			this->setPosMs( m_currentSoundPosition );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::resume()
	{
		if( m_sourceID == 0 )
		{
			return;
		}

		this->play_();
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::listenSoundNodeStopped()
	{
		if( m_playing == false )
		{
			return;
		}

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
				this->play_();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::listenSoundNodePaused()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::prepareSound_( const ConstString& _pakName, const WString& _file, const ConstString& _codec )
	{
		this->stop();
		//_release();

		m_buffer = Holder<SoundServiceInterface>::get()
			->createSoundBufferFromFile( _pakName, _file, _codec, true );

		if( m_buffer == 0 )
		{
			MENGE_LOG_ERROR( "Warning: AmplifierService can't load sample '%S'"
				, _file.c_str() 
				);

			return;			
		}

		m_sourceID = Holder<SoundServiceInterface>::get()
			->createSoundSource( false, m_buffer, true );

		if( m_sourceID == 0 )
		{
			MENGE_LOG_ERROR( "Warning: AmplifierService '%S' can't create sound source"
				, _file.c_str()
				);

			return;
		}
		
		Holder<SoundServiceInterface>::get()
			->setSourceListener( m_sourceID, this );
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::release_()
	{
		Holder<SoundServiceInterface>::get()
			->releaseSoundSource( m_sourceID );

		Holder<SoundServiceInterface>::get()
			->releaseSoundBuffer( m_buffer );

		m_sourceID = 0;
		m_buffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& AmplifierService::getPlayTrack() const
	{
		return m_currentPlaylistName;
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::setVolume( float _volume )
	{
		//m_volumeOverride = m_volume;
		Holder<SoundServiceInterface>::get()
			->setMusicVolume( _volume );
	}
	//////////////////////////////////////////////////////////////////////////
	float AmplifierService::getPosMs() const
	{
		if( m_sourceID == 0 )
		{
			return 0.0f;
		}

		float pos = Holder<SoundServiceInterface>::get()
			->getPosMs( m_sourceID );
		
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::setPosMs( float _posMs )
	{
		if( m_sourceID != 0 )
		{
			Holder<SoundServiceInterface>::get()
				->setPosMs( m_sourceID, _posMs );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
