#	include "Amplifier.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Playlist.h"

#	include "ResourcePlaylist.h"

#	include "Core/MemoryHelper.h"

#   include "Logger/Logger.h"

#	include <s3eAudio.h>
#	include <s3eDevice.h>

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
	static int32 s_Amplifier_AudioCallback_Stop( s3eAudioCallbackData * _data, Amplifier * _amplifier )
	{
		(void)_data;

		_amplifier->onSoundStop();

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::initialize()
	{
		LOGGER_INFO(m_serviceProvider)( "Starting Marmalade Amplifier..." );

		int32 available = s3eAudioGetInt( S3E_AUDIO_AVAILABLE );
		
		if( available == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Unavailable..."
				);

			return false;
		}

		s3eResult result = s3eAudioRegister( S3E_AUDIO_STOP, (s3eCallback)&s_Amplifier_AudioCallback_Stop, this );

		if( result != S3E_RESULT_SUCCESS )
		{
			s3eAudioError s3eAudio_error = s3eAudioGetError();
			const char * s3eAudio_string = s3eAudioGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("Amplifier::initialize: invalid register callback %d [%s]"
				, s3eAudio_error
				, s3eAudio_string
				);

			return false;
		}

		SOUND_SERVICE(m_serviceProvider)
			->addSoundVolumeProvider( this );

#	define MARMALADE_AMPLIFIER_SUPPORT_CODEC( Codec )\
	LOGGER_WARNING(m_serviceProvider)("Amplifier::initialize: " #Codec " %s"\
		, s3eAudioIsCodecSupported( Codec ) ? "support" : "unsupport!" )

		MARMALADE_AMPLIFIER_SUPPORT_CODEC( S3E_AUDIO_CODEC_MIDI );
		MARMALADE_AMPLIFIER_SUPPORT_CODEC( S3E_AUDIO_CODEC_MP3 );
		MARMALADE_AMPLIFIER_SUPPORT_CODEC( S3E_AUDIO_CODEC_AAC );
		MARMALADE_AMPLIFIER_SUPPORT_CODEC( S3E_AUDIO_CODEC_AACPLUS );
		MARMALADE_AMPLIFIER_SUPPORT_CODEC( S3E_AUDIO_CODEC_QCP );
		MARMALADE_AMPLIFIER_SUPPORT_CODEC( S3E_AUDIO_CODEC_PCM );
		MARMALADE_AMPLIFIER_SUPPORT_CODEC( S3E_AUDIO_CODEC_SPF );
		MARMALADE_AMPLIFIER_SUPPORT_CODEC( S3E_AUDIO_CODEC_AMR );
		MARMALADE_AMPLIFIER_SUPPORT_CODEC( S3E_AUDIO_CODEC_MP4 );

#	undef AMPLIFIER_SUPPORT_CODEC
		
		return true;			
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::finalize()
	{
		SOUND_SERVICE(m_serviceProvider)
			->removeSoundVolumeProvider( this );

		s3eAudioUnRegister( S3E_AUDIO_STOP, (s3eCallback)&s_Amplifier_AudioCallback_Stop );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::loadPlayList_( const ConstString& _playlistResource )
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
	bool Amplifier::playTrack( const ConstString& _playlistResource, uint32_t _index, float _pos, bool _looped )
	{
		if( this->loadPlayList_( _playlistResource ) == false )
		{
			return false;
		}

		if( m_currentPlayList == nullptr )
		{
			return false;
		}

		m_currentPlayList->setLooped1( _looped );
		m_currentPlayList->setTrack( _index );

		if( m_play == true )
		{
			return true;
		}

		if( this->preparePlay_( _pos ) == false )
		{
			return false;
		}

		m_play = true;

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
			return 0;
		}

		uint32_t index = m_currentPlayList->currentTrackIndex();

		return index;
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

		if( this->play_( category, track->path, track->codec, track->external, _pos ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::shuffle( const ConstString & _playlist )
	{
		if( this->loadPlayList_( _playlist ) == false )
		{
			return false;
		}

		if( m_currentPlayList == nullptr )
		{
			return false;
		}

		m_currentPlayList->shuffle();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::stop()
	{
		m_play = false;

		if( s3eAudioIsPlaying() == S3E_FALSE )
		{
			return;
		}

		s3eAudioStop();				
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::pause()
	{
		m_play = false;

		int32 s3eAudio_status = s3eAudioGetInt( S3E_AUDIO_STATUS );
		
		if( s3eAudio_status == -1 )
		{
			s3eAudioError s3eAudio_error = s3eAudioGetError();
			const char * s3eAudio_string = s3eAudioGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("Amplifier::pause invalid get status %d [%s]"
				, s3eAudio_error
				, s3eAudio_string
				);

			return false;
		}
		
		if( s3eAudio_status != S3E_AUDIO_PLAYING )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::pause invalid status S3E_AUDIO_PLAYING != %d"
				, s3eAudio_status
				);

			return false;
		}

		s3eResult result = s3eAudioPause();

		if( result == S3E_RESULT_ERROR )
		{
			s3eAudioError s3eAudio_error = s3eAudioGetError();
			const char * s3eAudio_string = s3eAudioGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("Amplifier::pause invalid s3eAudioPause %d [%s]"
				, s3eAudio_error
				, s3eAudio_string
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::resume()
	{
        m_play = true;

		int32 s3eAudio_status = s3eAudioGetInt( S3E_AUDIO_STATUS );

		if( s3eAudio_status == -1 )
		{
			s3eAudioError s3eAudio_error = s3eAudioGetError();
			const char * s3eAudio_string = s3eAudioGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("Amplifier::resume invalid get status %d [%s]"
				, s3eAudio_error
				, s3eAudio_string
				);

			return false;
		}

		if( s3eAudio_status != S3E_AUDIO_PAUSED )
		{
			LOGGER_ERROR(m_serviceProvider)("Amplifier::resume invalid status S3E_AUDIO_PAUSED != %d"
				, s3eAudio_status
				);

			return false;
		}

		s3eResult result = s3eAudioResume();

		if( result == S3E_RESULT_ERROR )
		{
			s3eAudioError s3eAudio_error = s3eAudioGetError();
			const char * s3eAudio_string = s3eAudioGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("Amplifier::resume invalid s3eAudioResume %d [%s]"
				, s3eAudio_error
				, s3eAudio_string
				);

			return false;
		}

		return true; 
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::onSoundStop()
	{
		if( m_play == false )
		{
			return;
		}

		m_audioMemory = nullptr;

        m_currentPlayList->next();

		const TrackDesc * track = m_currentPlayList->getTrack();
		
        if( track != nullptr )
        {
            const ConstString & category = m_currentPlayList->getCategory();
            
			if( this->play_( category, track->path, track->codec, track->external, 0.f ) == false )
			{
				return;
			}

			m_play = true;
        }		
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::play_( const ConstString& _pakName, const FilePath& _filePath, const ConstString& _codec, bool _external, float _pos )
	{
		this->stop();

		if( _external == false )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
				->openInputFile( _pakName, _filePath, false );

			if( stream == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: invalid open sound '%s:%s'"
					, _pakName.c_str()
					, _filePath.c_str()
					);

				return false;
			}
						
			
			m_audioMemory = Helper::createMemoryStream( m_serviceProvider, stream );

			if( m_audioMemory == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: invalid create memory '%s:%s'"
					, _pakName.c_str()
					, _filePath.c_str()
					);

				return false;
			}

			size_t buffer_size;
			void * buffer_memory = m_audioMemory->getMemory( buffer_size );

			s3eResult result_play = s3eAudioPlayFromBuffer( const_cast<void *>(buffer_memory), buffer_size, 1 );

			if( result_play == S3E_RESULT_ERROR )
			{
				s3eAudioError s3eAudio_error = s3eAudioGetError();
				const char * s3eAudio_string = s3eAudioGetErrorString();

				LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: can't play internal audio '%s:%s' error %d [%s]"
					, _pakName.c_str()
					, _filePath.c_str()
					, s3eAudio_error
					, s3eAudio_string
					);

				return false;
			}	
		}
		else
		{
			//ConstString fullPath;
			//if( Helper::makeFullPath( m_serviceProvider, _pakName, _filePath, fullPath ) == false )
			//{
			//	LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: can't make full path for external audio '%s:%s'"
			//		, _pakName.c_str()
			//		, _filePath.c_str()
			//		);

			//	return false;
			//}
						
			s3eResult result_play = s3eAudioPlay( _filePath.c_str(), 1 );

			if( result_play == S3E_RESULT_ERROR )
			{
				s3eAudioError s3eAudio_error = s3eAudioGetError();
				const char * s3eAudio_string = s3eAudioGetErrorString();

				LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: can't play external audio '%s:%s' error %d [%s]"
					, _pakName.c_str()
					, _filePath.c_str()
					, s3eAudio_error
					, s3eAudio_string
					);

				return false;
			}	
		}

		int32 s3e_pos = (int32)_pos;

		if( s3e_pos != 0 )
		{
			// W/o this, there is sound bugs during playback on iOS.
			s3eDeviceYield( 0 ); 

			s3eResult result_position = s3eAudioSetInt( S3E_AUDIO_POSITION, s3e_pos );

			if( result_position == S3E_RESULT_ERROR )
			{
				s3eAudioError s3eAudio_error = s3eAudioGetError();
				const char * s3eAudio_string = s3eAudioGetErrorString();

				LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: can't '%s:%s' set pos %d error %d [%s]"
					, _pakName.c_str()
					, _filePath.c_str()
					, s3e_pos
					, s3eAudio_error
					, s3eAudio_string
					);
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Amplifier::getPlayTrack() const
	{
		return m_currentPlaylistName;
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getPosMs() const
	{
		int32 s3e_pos = s3eAudioGetInt( S3E_AUDIO_POSITION );

		if( s3e_pos == -1 )
		{
			s3eAudioError s3eAudio_error = s3eAudioGetError();
			const char * s3eAudio_string = s3eAudioGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("Amplifier::getPosMs invalid get audio position %d [%s]"
				, s3eAudio_error
				, s3eAudio_string
				);

			return 0.f;
		}

		float pos = (float)s3e_pos;

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::setPosMs( float _posMs )
	{
		int32 s3e_pos = (int32)_posMs;
		 
		s3eResult result = s3eAudioSetInt( S3E_AUDIO_POSITION, s3e_pos );

		if( result == S3E_RESULT_ERROR )
		{
			s3eAudioError s3eAudio_error = s3eAudioGetError();
			const char * s3eAudio_string = s3eAudioGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("Amplifier::setPosMs invalid set S3E_AUDIO_POSITION %d error %d [%s]"
				, s3e_pos
				, s3eAudio_error
				, s3eAudio_string
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::onSoundChangeVolume( float _sound, float _music, float _voice )
	{
		int32 s3e_volume = (int32)(_music * float(S3E_AUDIO_MAX_VOLUME));

		s3eResult result = s3eAudioSetInt( S3E_AUDIO_VOLUME, s3e_volume );

		if( result == S3E_RESULT_ERROR )
		{
			s3eAudioError s3eAudio_error = s3eAudioGetError();
			const char * s3eAudio_string = s3eAudioGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("Amplifier::onSoundChangeVolume invalid set S3E_AUDIO_VOLUME %d error %d [%s]"
				, s3e_volume
				, s3eAudio_error
				, s3eAudio_string
				);
		}
	}
}
