#	include "Amplifier.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Kernel/ResourceMusic.h"

#	include "Core/MemoryHelper.h"

#   include "Logger/Logger.h"

#	include <s3eAudio.h>
#	include <s3eDevice.h>

#	include <cmath>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY(Amplifier, Menge::Amplifier);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
		: m_volume(1.f)
		, m_play(false)
		, m_loop(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Amplifier::~Amplifier()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static int32 s_Amplifier_AudioCallback_Stop( s3eAudioCallbackData * _data, Amplifier * _amplifier )
	{
		(void)_data;

		_amplifier->onSoundStop();

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::_initialize()
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
	void Amplifier::_finalize()
	{
		SOUND_SERVICE(m_serviceProvider)
			->removeSoundVolumeProvider( this );

		s3eAudioUnRegister( S3E_AUDIO_STOP, (s3eCallback)&s_Amplifier_AudioCallback_Stop );

		this->stop();
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
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::playMusic( const ConstString & _resourceMusic, float _pos, bool _looped )
	{
		this->stop();

		m_loop = _looped;

		ResourceMusicPtr resourceMusic = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceMusicPtr>( _resourceMusic );

		if( resourceMusic == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Amplifier::playMusic can't found resource '%s'"
				, _resourceMusic.c_str()
				);

			return false;
		}

		const ConstString & category = resourceMusic->getCategory();
		const FilePath & path = resourceMusic->getPath();
		const ConstString & codec = resourceMusic->getCodec();
		bool external = resourceMusic->isExternal();
		float volume = resourceMusic->getVolume();

		m_volume = volume;

		if( external == false )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
				->openInputFile( category, path, false );

			if( stream == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: invalid open sound '%s:%s'"
					, category.c_str()
					, path.c_str()
					);

				return false;
			}
						
			
			m_audioMemory = Helper::createMemoryStream( m_serviceProvider, stream );

			if( m_audioMemory == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: invalid create memory '%s:%s'"
					, category.c_str()
					, path.c_str()
					);

				return false;
			}
						
			void * buffer_memory = m_audioMemory->getMemory();
			size_t buffer_size = m_audioMemory->getSize();

			s3eResult result_play = s3eAudioPlayFromBuffer( const_cast<void *>(buffer_memory), buffer_size, 1 );

			if( result_play == S3E_RESULT_ERROR )
			{
				s3eAudioError s3eAudio_error = s3eAudioGetError();
				const char * s3eAudio_string = s3eAudioGetErrorString();

				LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: can't play internal audio '%s:%s' error %d [%s]"
					, category.c_str()
					, path.c_str()
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

			const Char * str_path = path.c_str();
						
			s3eResult result_play = s3eAudioPlay( str_path, 1 );

			if( result_play == S3E_RESULT_ERROR )
			{
				s3eAudioError s3eAudio_error = s3eAudioGetError();
				const char * s3eAudio_string = s3eAudioGetErrorString();

				LOGGER_ERROR(m_serviceProvider)("Amplifier::play_: can't play external audio '%s:%s' error %d [%s]"
					, category.c_str()
					, path.c_str()
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
					, category.c_str()
					, path.c_str()
					, s3e_pos
					, s3eAudio_error
					, s3eAudio_string
					);
			}
		}

		float commonVolume = SOUND_SERVICE( m_serviceProvider )
			->mixCommonVolume();

		float musicVolume = SOUND_SERVICE( m_serviceProvider )
			->mixMusicVolume();
		
		int32 s3e_volume = (int32)(m_volume * commonVolume * musicVolume * float( S3E_AUDIO_MAX_VOLUME ));

		s3eResult result = s3eAudioSetInt( S3E_AUDIO_VOLUME, s3e_volume );

		if( result == S3E_RESULT_ERROR )
		{
			s3eAudioError s3eAudio_error = s3eAudioGetError();
			const char * s3eAudio_string = s3eAudioGetErrorString();

			LOGGER_ERROR( m_serviceProvider )("Amplifier::onSoundChangeVolume invalid set S3E_AUDIO_VOLUME %d error %d [%s]"
				, s3e_volume
				, s3eAudio_error
				, s3eAudio_string
				);
		}

		return true;
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
	float Amplifier::getLengthMs() const
	{
		int32 s3e_duration = s3eAudioGetInt( S3E_AUDIO_DURATION );

		float duration = (float)s3e_duration;

		return duration;
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
		int32 s3e_volume = (int32)(m_volume * _music * float( S3E_AUDIO_MAX_VOLUME ));

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
