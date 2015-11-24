#	include "Amplifier.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Playlist.h"

#	include "ResourcePlaylist.h"

#   include "Logger/Logger.h"

#	include <cmath>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Amplifier, Menge::Amplifier);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
		: m_currentPlayList(nullptr)
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
	bool Amplifier::_initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::_finalize()
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
	bool Amplifier::playTrack( const ConstString& _playlistResource, uint32_t _index, float _pos, bool _looped )
	{
		if( this->loadPlayList_( _playlistResource ) == false )
		{
			return false;
		}

		if( m_currentPlayList == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::playTrack '%s' index %d pos %f loop %d invalid setup current play list"
				, _playlistResource.c_str()
				, _index
				, _pos
				, _looped
				);

			return false;
		}

		m_currentPlayList->setLooped1( _looped );
		m_currentPlayList->setTrack( _index );

		if( this->play2_( _pos ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::playTrack '%s' index %d pos %f loop %d invalid play"
				, _playlistResource.c_str()
				, _index
				, _pos
				, _looped
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Amplifier::getNumTracks() const
	{
		if( m_currentPlayList == nullptr )
		{
			return 0;
		}

        uint32_t numTracks = m_currentPlayList->numTracks();

		return numTracks;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Amplifier::getCurrentTrack() const
	{
		if( m_currentPlayList == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::getCurrentTrack invalid setup current track, return 0"
				);

			return 0;
		}

		uint32_t index = m_currentPlayList->currentTrackIndex();

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
			LOGGER_ERROR(m_serviceProvider)("Amplifier::play2_ invalid prepare play"
				);

			return false;
		}

		if( this->play_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::play2_ invalid play"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::preparePlay_( float _pos )
	{
		if( m_currentPlayList == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::preparePlay_ invalid current play list"
				);

			return false;
		}

		const TrackDesc * track = m_currentPlayList->getTrack();

		if( track == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::preparePlay_ invalid get track"
				);
			
			return false;
		}

		const ConstString & category = m_currentPlayList->getCategory();

		if( this->prepareSound_( category, track->path, track->codec, track->external, _pos ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::preparePlay_ invalid prepare sound"
				);

			return false;
		}

		if( m_sourceID == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::preparePlay_ invalid source ID"
				);

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
	bool Amplifier::shuffle( const ConstString & _playlist )
	{
		if( this->loadPlayList_( _playlist ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::shuffle invalid load play list %s"
				, _playlist.c_str()
				);

			return false;
		}

		if( m_currentPlayList == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::shuffle invalid setup current play list '%s'"
				, _playlist.c_str()
				);

			return false;
		}

		m_currentPlayList->shuffle();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::stop()
	{
		m_play = false;

		m_buffer = nullptr;

		if( m_sourceID != 0 )
		{
			uint32_t sourceId = m_sourceID;
			m_sourceID = 0;

			SOUND_SERVICE(m_serviceProvider)
                ->stop( sourceId );
		
			SOUND_SERVICE(m_serviceProvider)
				->releaseSoundSource( sourceId );			
		}				
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::pause()
	{
		if( m_sourceID == 0 )
		{
			return false;
		}

		m_play = false;

		SOUND_SERVICE(m_serviceProvider)
            ->pause( m_sourceID );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::resume()
	{
		if( m_sourceID == 0 )
		{
			return false;
		}

        m_play = true;

        SOUND_SERVICE(m_serviceProvider)
            ->play( m_sourceID );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::onSoundStop( uint32_t _id )
	{
		(void)_id;

		if( m_play == false )
		{
			return;
		}
				
		if( m_currentPlayList == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::onSoundStop current play list is nullptr"
				);

			return;
		}

        m_currentPlayList->next();
		
        const TrackDesc * track = m_currentPlayList->getTrack();

        if( track != nullptr )
        {
            const ConstString & category = m_currentPlayList->getCategory();
            
			if( this->prepareSound_( category, track->path, track->codec, track->external, 0.f ) == false )
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
	void Amplifier::onSoundPause( uint32_t _id )
	{
		(void)_id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::prepareSound_( const ConstString& _pakName, const FilePath& _file, const ConstString& _codec, bool _external, float _pos )
	{
		this->stop();
		//_release();

		FileGroupInterfacePtr fileGroup = FILE_SERVICE( m_serviceProvider )
			->getFileGroup( _pakName );

		if( fileGroup->isPacked() == false || _external == false )
		{
			m_buffer = SOUND_SERVICE( m_serviceProvider )
				->createSoundBufferFromFile( _pakName, _file, _codec, true );
		}
		else
		{ 
			m_buffer = SOUND_SERVICE( m_serviceProvider )
				->createSoundBufferFromFile( ConstString::none(), _file, _codec, true );
		}

		if( m_buffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::prepareSound_: AmplifierService can't load sample '%s'"
				, _file.c_str() 
				);

			return false;			
		}

		m_sourceID = SOUND_SERVICE(m_serviceProvider)
            ->createSoundSource( false, m_buffer, ESST_MUSIC, true );

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
