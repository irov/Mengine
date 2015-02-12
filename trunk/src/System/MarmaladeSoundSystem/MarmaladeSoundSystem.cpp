#	include "MarmaladeSoundSystem.h"

#	include "MarmaladeSoundConfig.h"
#	include "MarmaladeSoundError.h"
#	include "MarmaladeSoundSource.h"

#	include "Logger/Logger.h"

#	include <math.h>

#	include <s3eDevice.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Menge::SoundSystemInterface, Menge::MarmaladeSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSystem::MarmaladeSoundSystem()
		: m_serviceProvider(nullptr)
		, m_isDeviceStereo(true)
		, m_soundOutputFrequence(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSystem::~MarmaladeSoundSystem()
	{     

	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeSoundSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeSoundSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeSoundSystem::newSound( MarmaladeSoundSource * _source )
	{
		for( uint32_t i = 0; i != MENGINE_MARMALADE_SOUND_MAX_COUNT; ++i )
		{
			volatile SoundMemoryDesc & desc = m_soundMemoryDesc[i];
			
			if( desc.memory != nullptr )
			{
				continue;
			}

			desc.source = _source;

			return i;
		}

		return (uint32_t)-1;
	}
	//////////////////////////////////////////////////////////////////////////
	volatile SoundMemoryDesc * MarmaladeSoundSystem::getSound( uint32_t _index )
	{
		return m_soundMemoryDesc + _index;
	}
	//////////////////////////////////////////////////////////////////////////
	static uint32_t s_getAvailableDesc( volatile SoundMemoryDesc ** _out, volatile SoundMemoryDesc * _from )
	{
		uint32_t availableCount = 0;

		for( uint32_t index_desc = 0; index_desc != MENGINE_MARMALADE_SOUND_MAX_COUNT; ++index_desc )
		{
			volatile SoundMemoryDesc & desc = _from[index_desc];

			if( desc.play == false || desc.end == true || desc.pause == true )
			{
				continue;
			}

			if( desc.stop == true )
			{
				desc.play = false;
				desc.end = true;

				continue;
			}

			if( desc.volume == 0 )
			{
				continue;
			}

			_out[availableCount] = &desc;
			++availableCount;
		}

		return availableCount;
	}
	//////////////////////////////////////////////////////////////////////////
	static int32 s_AudioCallbackMono( void * _sys, void * _user )
	{
		s3eSoundGenAudioInfo * info = (s3eSoundGenAudioInfo *)_sys;
		volatile SoundMemoryDesc * soundMemoryDesc = (volatile SoundMemoryDesc *)_user;

		info->m_EndSample = 0;

		int16 * target = info->m_Target;

		volatile SoundMemoryDesc * availableDesc[MENGINE_MARMALADE_SOUND_MAX_COUNT];
		uint32_t availableCount = s_getAvailableDesc( availableDesc, soundMemoryDesc );				

		if( availableCount == 0 )
		{
			memset( target, 0, 1 * 1 * sizeof(int16) );

			return 1;
		}

		uint32 numSamples = info->m_NumSamples;

		if( info->m_Mix == 0 )
		{
			memset( target, 0, numSamples * 1 * sizeof(int16) );
		}
				
		for( uint32_t sample = 0; sample != numSamples; ++sample )
		{		
			int32 origM = (int32)*(target+0);

			for( uint32_t index = 0; index != availableCount; )
			{
				volatile SoundMemoryDesc * desc = availableDesc[index];

				uint32 sampleStep = desc->sampleStep;

				uint8 volume = desc->volume;

				uint32 outPosLeft = desc->carriage * sampleStep;

				int32 yMono = (desc->memory[outPosLeft + 0] * volume) >> 8;

				origM += yMono;

				++desc->carriage;

				if( desc->carriage == desc->size )
				{
					if( desc->count == -1 || --desc->count > 0 )
					{
						desc->carriage = 0;
					}
					else
					{
						desc->play = false;
						desc->end = true;
					}

					--availableCount;
					availableDesc[index] = availableDesc[availableCount];
				}
				else
				{
					++index;
				}
			}

			*target++ = s_clipToInt16(origM);
		}

		return info->m_NumSamples;
	}
	//////////////////////////////////////////////////////////////////////////
	static int32 s_AudioCallbackStereo( void * _sys, void * _user )
	{
		s3eSoundGenAudioInfo * info = (s3eSoundGenAudioInfo *)_sys;
		volatile SoundMemoryDesc * soundMemoryDesc = (volatile SoundMemoryDesc *)_user;

		info->m_EndSample = 0;

		volatile SoundMemoryDesc * availableDesc[MENGINE_MARMALADE_SOUND_MAX_COUNT];
		uint32_t availableCount = s_getAvailableDesc( availableDesc, soundMemoryDesc );		

		int16 * target = info->m_Target;

		if( availableCount == 0 )
		{
			memset( target, 0, 1 * 2 * sizeof(int16) );

			return 1;
		}
		
		uint32 numSamples = info->m_NumSamples;

		if( info->m_Mix == 0 )
		{
			memset( target, 0, numSamples * 2 * sizeof(int16) );
		}

		for( uint32_t sample = 0; sample != numSamples; ++sample )
		{		
			int32 origL = (int32)*(target+0);
			int32 origR = (int32)*(target+1);

			for( uint32_t index = 0; index != availableCount; )
			{
				volatile SoundMemoryDesc * desc = availableDesc[index];

				uint32 sampleStep = desc->sampleStep;

				uint8 volume = desc->volume;

				uint32 outPosLeft = desc->carriage * sampleStep;

				int32 yLeft = (desc->memory[outPosLeft + 0] * volume) >> 8;
				int32 yRight = (desc->memory[outPosLeft + 1] * volume) >> 8;

				origL += yLeft;
				origR += yRight;

				++desc->carriage;

				if( desc->carriage == desc->size )
				{
					if( desc->count == -1 || --desc->count > 0 )
					{
						desc->carriage = 0;
					}
					else
					{
						desc->play = false;
						desc->end = true;
					}

					--availableCount;
					availableDesc[index] = availableDesc[availableCount];
				}
				else
				{
					++index;
				}
			}

			*target++ = s_clipToInt16(origL);
			*target++ = s_clipToInt16(origR);
		}

		return info->m_NumSamples;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSystem::initialize()
	{
		LOGGER_INFO(m_serviceProvider)( "Starting Marmalade Sound System..." );

		int32 propertyStereoEnabled = s3eSoundGetInt( S3E_SOUND_STEREO_ENABLED );

		if( propertyStereoEnabled == 1 )
		{
			m_isDeviceStereo = true;
		}
		else if( propertyStereoEnabled == 0 )
		{
			m_isDeviceStereo = false;
		}
		else
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			m_isDeviceStereo = false;
		}

		if( s3eSoundSetInt( S3E_SOUND_DEFAULT_FREQ, 44100 ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}

		m_soundOutputFrequence = s3eSoundGetInt( S3E_SOUND_OUTPUT_FREQ );

		if( m_soundOutputFrequence == -1 )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			m_soundOutputFrequence = 22050;
		}

		for( uint32_t i = 0; i != MENGINE_MARMALADE_SOUND_MAX_COUNT; ++i )
		{
			volatile SoundMemoryDesc & desc = m_soundMemoryDesc[i];
						
			desc.carriage = 0;
			desc.size = 0;
			desc.memory = nullptr;
			desc.sampleStep = 0;
			desc.volume = 0;
			desc.count = 0;
			desc.play = false;
			desc.pause = false;
			desc.stop = false;
			desc.end = false;
		}

		int32 numChannels = s3eSoundGetInt( S3E_SOUND_NUM_CHANNELS );

		LOGGER_WARNING(m_serviceProvider)("MarmaladeSoundSystem sound channels %d"
			, numChannels
			);

		int soundChannel = s3eSoundGetFreeChannel();

		if( s3eSoundChannelRegister( soundChannel, S3E_CHANNEL_GEN_AUDIO, &s_AudioCallbackMono, (void *)m_soundMemoryDesc ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			return false;
		}

		if( m_isDeviceStereo == true )
		{
			if( s3eSoundChannelRegister( soundChannel, S3E_CHANNEL_GEN_AUDIO_STEREO, &s_AudioCallbackStereo, (void *)m_soundMemoryDesc ) == S3E_RESULT_ERROR )
			{
				MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

				return false;
			}
		}

		int16 dummydata[16];
		if( s3eSoundChannelPlay( soundChannel, dummydata, 8, 0, 0 ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			return false;
		}

		m_soundChannel = soundChannel;
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSystem::finalize()
	{
		if( s3eSoundChannelStop( m_soundChannel ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}

		s3eDeviceYield(50);
		
		if( m_isDeviceStereo == true )
		{
			if( s3eSoundChannelUnRegister( m_soundChannel, S3E_CHANNEL_GEN_AUDIO_STEREO ) == S3E_RESULT_ERROR )
			{
				MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
			}
		}

		if( s3eSoundChannelUnRegister( m_soundChannel, S3E_CHANNEL_GEN_AUDIO ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSystem::update()
	{
		for( uint32 index_desc = 0; index_desc != MENGINE_MARMALADE_SOUND_MAX_COUNT; ++index_desc )
		{
			volatile SoundMemoryDesc & desc = m_soundMemoryDesc[index_desc];

			if( desc.end == false )
			{
				continue;
			}

			stdex_free( desc.memory );
			
			desc.carriage = 0;
			desc.size = 0;
			desc.memory = nullptr;
			desc.sampleStep = 0;
			desc.volume = 0;
			desc.count = 0;
			desc.pause = false;
			desc.play = false;
			desc.stop = false;
			desc.end = false;

			desc.source->complete();
			desc.source = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSystem::setupFilter_()
	{
		//int minimumFrequence = MENGINE_MARMALADE_SOUND_INPUT_FREQUENCE < m_soundOutputFrequence ? MENGINE_MARMALADE_SOUND_INPUT_FREQUENCE : m_soundOutputFrequence;
		//double fc = ((double)minimumFrequence / 2.0) / (double)m_soundOutputFrequence; // half the input sample rate (eg nyquist limit of input)

		//// Generate filter coefficients
		//s_wsfirLP( m_filterCoefficients, ESWC_BLACKMAN, fc );

		// Sample rate conversion parameters
		//int gcd = s_GCD( MENGINE_MARMALADE_SOUND_INPUT_FREQUENCE, m_soundOutputFrequence );
		
		//m_W = MENGINE_MARMALADE_SOUND_INPUT_FREQUENCE / gcd;
		//m_L = m_soundOutputFrequence / gcd;		
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSystem::onTurnSound( bool _turn )
	{
		if( _turn == false )
		{
			s3eSoundPauseAllChannels();
		}
		else
		{
			s3eSoundResumeAllChannels();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr MarmaladeSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
	{
		_isStream = false;

		MarmaladeSoundBufferPtr base;

		if( _isStream == false )
		{
			MarmaladeSoundBufferMemory * buffer = m_poolOALSoundBuffer.createObjectT();

			buffer->setServiceProvider( m_serviceProvider );

			base = buffer;
		}
		else
		{
			MarmaladeSoundBufferStream * buffer = m_poolOALSoundBufferStream.createObjectT();

			buffer->setServiceProvider( m_serviceProvider );

			base = buffer;
		}

		if( base->load( _soundDecoder ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem: Failed to create sound buffer from stream" 
				);

			return nullptr;
		}

		return base;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterfacePtr MarmaladeSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
	{
		//OALSoundSource* soundSource = m_soundSources.get();
		MarmaladeSoundSource * soundSource = m_poolOALSoundSource.createObjectT();

        soundSource->initialize( m_serviceProvider, this );
        		
		soundSource->setSoundBuffer( _buffer );
		
		return soundSource;
	}
}	// namespace Menge

