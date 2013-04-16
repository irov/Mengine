#	include "AmplifierService.h"

#   include "Interface/ResourceInterface.h"

#   include "Logger/Logger.h"

#	include "Playlist.h"

#	include "ResourcePlaylist.h"

#	include <cmath>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AmplifierService, Menge::AmplifierServiceInterface, Menge::AmplifierService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AmplifierService::AmplifierService()
		: m_serviceProvider(NULL)
        , m_sourceID(0)
		, m_buffer(NULL)
		, m_currentPlayList(0)
		, m_volume(1.f)
		, m_volumeOverride(1.f)
		, m_playing(false)
		, m_currentSoundPosition(0.f)
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
    void AmplifierService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * AmplifierService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool AmplifierService::loadPlayList_( const ConstString& _playlistResource )
	{
		TMapPlayList::iterator it = m_mapPlayLists.find( _playlistResource );

		if ( it == m_mapPlayLists.end() )
		{			
			Playlist * playlist = new Playlist( m_serviceProvider );

            if( playlist->initialize( _playlistResource ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "Amplifier: no found playlist with name '%s'"
                    , _playlistResource.c_str()
                    );

                delete playlist;

                return false;
            }

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

        m_currentPlaylistName.clear();

		if( m_sourceID != 0 )
		{
			SOUND_SERVICE(m_serviceProvider)->stop( m_sourceID );
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

		float musicVolume = SOUND_SERVICE(m_serviceProvider)->getMusicVolume();

		if( SOUND_SERVICE(m_serviceProvider)->setSourceVolume( m_sourceID, musicVolume ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AmplifierService::playTrack invalid set source volume %ls"
				, desc->path.c_str()
				);
		}

		SOUND_SERVICE(m_serviceProvider)->play( m_sourceID );
		
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
			LOGGER_ERROR(m_serviceProvider)( "AmplifierService: no found playlist with name '%s'"
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
		SOUND_SERVICE(m_serviceProvider)->play( m_sourceID );

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
			SOUND_SERVICE(m_serviceProvider)->stop( m_sourceID );

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

		SOUND_SERVICE(m_serviceProvider)->pause( m_sourceID );
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
				this->prepareSound_( category, track->path, track->codec );
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
	void AmplifierService::prepareSound_( const ConstString& _pakName, const FilePath& _file, const ConstString& _codec )
	{
		this->stop();
		//_release();

		m_buffer = SOUND_SERVICE(m_serviceProvider)->createSoundBufferFromFile( _pakName, _file, _codec, true );

		if( m_buffer == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "AmplifierService::prepareSound_: AmplifierService can't load sample '%s'"
				, _file.c_str() 
				);

			return;			
		}

		m_sourceID = SOUND_SERVICE(m_serviceProvider)->createSoundSource( false, m_buffer, EST_MUSIC, true );

		if( m_sourceID == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "AmplifierService::prepareSound_: AmplifierService can't create sound source '%s'"
				, _file.c_str()
				);

			return;
		}
		
		SOUND_SERVICE(m_serviceProvider)->setSourceListener( m_sourceID, this );
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::release_()
	{
		SOUND_SERVICE(m_serviceProvider)->releaseSoundSource( m_sourceID );
        m_sourceID = 0;

		SOUND_SERVICE(m_serviceProvider)->releaseSoundBuffer( m_buffer );		
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
		SOUND_SERVICE(m_serviceProvider)->setMusicVolume( _volume );
	}
	//////////////////////////////////////////////////////////////////////////
	float AmplifierService::getPosMs() const
	{
		if( m_sourceID == 0 )
		{
			return 0.0f;
		}

		float pos = SOUND_SERVICE(m_serviceProvider)->getPosMs( m_sourceID );
		
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::setPosMs( float _posMs )
	{
		if( m_sourceID != 0 )
		{
			SOUND_SERVICE(m_serviceProvider)->setPosMs( m_sourceID, _posMs );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
