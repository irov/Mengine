#	include "Amplifier.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Playlist.h"

#	include "ResourcePlaylist.h"

#   include "Logger/Logger.h"

#	include <cmath>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY(Amplifier, Menge::AmplifierInterface, Menge::Amplifier);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
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
	Amplifier::~Amplifier()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Amplifier::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Amplifier::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::finalize()
	{
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

		m_mapPlayLists.clear();

		m_currentPlayList = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::loadPlayList_( const ConstString& _playlistResource )
	{
		TMapPlayList::iterator it_found = m_mapPlayLists.find( _playlistResource );

		if( it_found == m_mapPlayLists.end() )
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

			it_found = m_mapPlayLists.insert( std::make_pair( _playlistResource, playlist ) ).first;
		}

		m_currentPlaylistName = _playlistResource;
		m_currentPlayList = it_found->second;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::playTrack( const ConstString& _playlistResource, size_t _index, float _pos, bool _looped )
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
	size_t Amplifier::getNumTracks() const
	{
		if( m_currentPlayList == nullptr )
		{
			return 0;
		}

        size_t numTracks = m_currentPlayList->numTracks();

		return numTracks;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Amplifier::getCurrentTrack() const
	{
		if( m_currentPlayList == nullptr )
		{
			return 0;
		}

		size_t index = m_currentPlayList->currentTrackIndex();

		return index;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::play2_( float _pos )
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
	bool Amplifier::preparePlay_( float _pos )
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
	bool Amplifier::play_()
	{
		if( SOUND_SERVICE(m_serviceProvider)->play( m_sourceID ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::play_ invalid play %d"
				, m_sourceID
				);

			return false;
		}

		m_play = true;

		return true;
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
		m_play = false;

		m_buffer = nullptr;

		if( m_sourceID != 0 )
		{
			size_t sourceId = m_sourceID;
			m_sourceID = 0;

			SOUND_SERVICE(m_serviceProvider)
                ->stop( sourceId );
		
			SOUND_SERVICE(m_serviceProvider)
				->releaseSoundSource( sourceId );			
		}				
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::pause()
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
	void Amplifier::resume()
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
	void Amplifier::onSoundStop( size_t _id )
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
	void Amplifier::onSoundPause( size_t _id )
	{
		(void)_id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::prepareSound_( const ConstString& _pakName, const FilePath& _file, const ConstString& _codec, float _pos )
	{
		this->stop();
		//_release();

		m_buffer = SOUND_SERVICE(m_serviceProvider)
            ->createSoundBufferFromFile( _pakName, _file, _codec, true );

		if( m_buffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::prepareSound_: AmplifierService can't load sample '%s'"
				, _file.c_str() 
				);

			return false;			
		}

		m_sourceID = SOUND_SERVICE(m_serviceProvider)
            ->createSoundSource( false, m_buffer, EST_MUSIC, true );

		if( m_sourceID == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::prepareSound_: AmplifierService can't create sound source '%s'"
				, _file.c_str()
				);

			return false;
		}

		if( SOUND_SERVICE(m_serviceProvider)
			->setPosMs( m_sourceID, _pos ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::prepareSound_: AmplifierService can't set sound '%s' pos '%f'"
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
	const ConstString& Amplifier::getPlayTrack() const
	{
		return m_currentPlaylistName;
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getPosMs() const
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
	void Amplifier::setPosMs( float _posMs )
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
