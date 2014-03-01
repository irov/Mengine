#	include "AmplifierService.h"

#   include "Interface/ResourceInterface.h"

#   include "Logger/Logger.h"

#	include "Consts.h"

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
		: m_serviceProvider(nullptr)
		, m_currentPlayList(nullptr)
        , m_sourceID(0)
		, m_buffer(nullptr)
		, m_volume(1.f)
		, m_volumeOverride(1.f)
		, m_play(false)
        , m_turn(true)
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
			Playlist * playlist = it->second;
			
			delete playlist;
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

		if( it == m_mapPlayLists.end() )
		{			
			Playlist * playlist = new Playlist( m_serviceProvider );

            if( playlist->initialize( _playlistResource ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("Amplifier: no found playlist with name '%s'"
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
	bool AmplifierService::playTrack( const ConstString& _playlistResource, size_t _index, float _pos, bool _looped )
	{
		if( this->loadPlayList_( _playlistResource ) == false )
		{
			return false;
		}

		m_currentPlayList->setLooped1( _looped );
		m_currentPlayList->setTrack( _index );

		if( this->play2_( _pos ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t AmplifierService::getNumTracks() const
	{
		if( m_currentPlayList == nullptr )
		{
			return 0;
		}

        size_t numTracks = m_currentPlayList->numTracks();

		return numTracks;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t AmplifierService::getCurrentTrack() const
	{
		if( m_currentPlayList == nullptr )
		{
			return 0;
		}

		size_t index = m_currentPlayList->currentTrackIndex();

		return index;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AmplifierService::play2_( float _pos )
	{
        if( m_play == true )
        {
            return true;
        }

		if( this->preparePlay_( _pos ) == false )
		{
			return false;
		}

		if( this->play_() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AmplifierService::preparePlay_( float _pos )
	{
		if( m_currentPlayList == nullptr )
		{
			return false;
		}

		const TrackDesc * track = m_currentPlayList->getTrack();

		if( track == nullptr )
		{
			return false;
		}

		const ConstString & category = m_currentPlayList->getCategory();
		if( this->prepareSound_( category, track->path, track->codec, _pos ) == false )
		{
			return false;
		}

		if( m_sourceID == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AmplifierService::play_()
	{
		if( SOUND_SERVICE(m_serviceProvider)->play( m_sourceID ) == false )
		{
			return false;
		}

		m_play = true;

		return true;
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
		m_play = false;

		if( m_sourceID != 0 )
		{
			SOUND_SERVICE(m_serviceProvider)
                ->stop( m_sourceID );

			this->release_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::pause()
	{
		if( m_sourceID == 0 )
		{
			return;
		}

		m_play = false;

		SOUND_SERVICE(m_serviceProvider)
            ->pause( m_sourceID );
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::resume()
	{
		if( m_sourceID == 0 )
		{
			return;
		}

        m_play = true;

        SOUND_SERVICE(m_serviceProvider)
            ->play( m_sourceID );
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::onSoundStop( size_t _id )
	{
		(void)_id;

		if( m_play == false )
		{
			return;
		}

		//if( m_currentPlayList->getLooped() == false )
		//{
        m_currentPlayList->next();
        const TrackDesc * track = m_currentPlayList->getTrack();

        if( track != nullptr )
        {
            const ConstString & category = m_currentPlayList->getCategory();
            
			if( this->prepareSound_( category, track->path, track->codec, 0.f ) == false )
			{
				return;
			}
        }

        if( m_sourceID != 0 )
        {
            //Holder<SoundEngine>::get()
            //	->setVolume( m_sourceID, Holder<SoundEngine>::get()->getMusicVolume() );
            this->play_();
        }
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::onSoundPause( size_t _id )
	{
		(void)_id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AmplifierService::prepareSound_( const ConstString& _pakName, const FilePath& _file, const ConstString& _codec, float _pos )
	{
		this->stop();
		//_release();

		m_buffer = SOUND_SERVICE(m_serviceProvider)
            ->createSoundBufferFromFile( _pakName, _file, _codec, true );

		if( m_buffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("AmplifierService::prepareSound_: AmplifierService can't load sample '%s'"
				, _file.c_str() 
				);

			return false;			
		}

		m_sourceID = SOUND_SERVICE(m_serviceProvider)
            ->createSoundSource( false, m_buffer, EST_MUSIC, true );

		if( m_sourceID == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("AmplifierService::prepareSound_: AmplifierService can't create sound source '%s'"
				, _file.c_str()
				);

			return false;
		}

		if( SOUND_SERVICE(m_serviceProvider)
			->setPosMs( m_sourceID, _pos ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AmplifierService::prepareSound_: AmplifierService can't set sound '%s' pos '%f'"
				, _file.c_str()
				, _pos
				);

			return false;
		}
		
		SOUND_SERVICE(m_serviceProvider)
            ->setSourceListener( m_sourceID, this );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::release_()
	{
		SOUND_SERVICE(m_serviceProvider)
            ->releaseSoundSource( m_sourceID );

        m_sourceID = 0;

    	m_buffer = nullptr;
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
		SOUND_SERVICE(m_serviceProvider)
            ->setMusicVolume( CONST_STRING(m_serviceProvider, Generic), _volume );
	}
	//////////////////////////////////////////////////////////////////////////
	float AmplifierService::getVolume() const
	{
		float volume = SOUND_SERVICE(m_serviceProvider)
			->getMusicVolume( CONST_STRING(m_serviceProvider, Generic) );

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////
	float AmplifierService::getPosMs() const
	{
		if( m_sourceID == 0 )
		{
			return 0.f;
		}

		float pos = SOUND_SERVICE(m_serviceProvider)
            ->getPosMs( m_sourceID );
		
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void AmplifierService::setPosMs( float _posMs )
	{
		if( m_sourceID == 0 )
		{
            return;
        }
		
        SOUND_SERVICE(m_serviceProvider)
            ->setPosMs( m_sourceID, _posMs );
	}
	//////////////////////////////////////////////////////////////////////////
}
