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
	float MarmaladeSoundSystem::carriageToPosition_( uint32 _carriage, uint32_t _frequency ) const
	{
		uint32_t gcd = s_GCD( _frequency, m_soundOutputFrequence );
		uint32_t W = _frequency / gcd;
		uint32_t L = m_soundOutputFrequence / gcd;

		float posMs = (float)(_carriage * 1000 * L / (_frequency * W));

		return posMs;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32 MarmaladeSoundSystem::positionToCarriage_( float _position, uint32_t _frequency ) const
	{
		uint32_t gcd = s_GCD( _frequency, m_soundOutputFrequence );
		uint32_t W = _frequency / gcd;
		uint32_t L = m_soundOutputFrequence / gcd;

		uint32 carriage = uint32_t(_position * _frequency) * W / (L * 1000);

		return carriage;
	}
	//////////////////////////////////////////////////////////////////////////	
	uint32_t MarmaladeSoundSystem::playSoundDesc( MarmaladeSoundSource * _source, float _position, float _volume, int32 _count )
	{
		for( uint32_t i = 0; i != MENGINE_MARMALADE_SOUND_MAX_COUNT; ++i )
		{
			volatile MarmaladeSoundMemoryDesc & desc = m_soundMemoryDesc[i];
			
			if( desc.memory != nullptr || desc.play == true )
			{
				continue;
			}

			SoundBufferInterfacePtr soundBuffer = _source->getSoundBuffer();

			if( soundBuffer == nullptr )
			{
				return INVALID_SOUND_ID;
			}

			const SoundDecoderInterfacePtr & decoder = soundBuffer->getDecoder();

			if( decoder->rewind() == false )
			{
				return INVALID_SOUND_ID;
			}

			const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

			void * memory_s3e = stdex_malloc( dataInfo->size );

			size_t size = decoder->decode( memory_s3e, dataInfo->size );

			if( size != dataInfo->size )
			{
				return INVALID_SOUND_ID;
			}

			desc.source = _source;

			uint32_t inputFrequency = dataInfo->frequency;

			uint32 carriage = this->positionToCarriage_( _position, inputFrequency );
			desc.carriage = carriage;
			
			uint32 carriage_max = this->positionToCarriage_( dataInfo->length, inputFrequency );
			desc.size = carriage_max;

			desc.memory = (int16 *)memory_s3e;

			desc.frequency = inputFrequency;

			uint32_t sampleSize = (dataInfo->stereo == true ? 2 : 1);
			desc.sampleStep = sampleSize;
			
			uint8 volume_s3e = (uint8)(_volume * (S3E_SOUND_MAX_VOLUME - 1));
			desc.volume = volume_s3e;

			desc.pause = false;
			desc.stop = false;
			desc.end = false;

			desc.count = _count;

			desc.play = true;
			
			return i;
		}

		return INVALID_SOUND_ID;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSystem::removeSoundDesc( uint32_t _id )
	{
		if( _id == INVALID_SOUND_ID )
		{
			return;
		}

		if( _id >= MENGINE_MARMALADE_SOUND_MAX_COUNT )
		{
			return;
		}

		volatile MarmaladeSoundMemoryDesc & desc = m_soundMemoryDesc[_id];

		desc.source = nullptr;
		desc.stop = true;
		desc.pause = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSystem::stopSoundDesc( uint32_t _id )
	{
		if( _id == INVALID_SOUND_ID )
		{
			return false;
		}

		if( _id >= MENGINE_MARMALADE_SOUND_MAX_COUNT )
		{
			return false;
		}

		volatile MarmaladeSoundMemoryDesc & desc = m_soundMemoryDesc[_id];

		desc.stop = true;
		desc.pause = false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSystem::pauseSoundDesc( uint32_t _id, float & _position )
	{
		if( _id == INVALID_SOUND_ID )
		{
			return false;
		}

		if( _id >= MENGINE_MARMALADE_SOUND_MAX_COUNT )
		{
			return false;
		}

		volatile MarmaladeSoundMemoryDesc & desc = m_soundMemoryDesc[_id];

		float position = this->carriageToPosition_( desc.carriage, desc.frequency );
		
		_position = position;

		desc.pause = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSystem::resumeSoundDesc( uint32_t _id, float _position )
	{
		if( _id == INVALID_SOUND_ID )
		{
			return false;
		}

		if( _id >= MENGINE_MARMALADE_SOUND_MAX_COUNT )
		{
			return false;
		}

		volatile MarmaladeSoundMemoryDesc & desc = m_soundMemoryDesc[_id];

		uint32 carriage = this->positionToCarriage_( _position, desc.frequency );

		desc.carriage = carriage;
		desc.pause = false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSystem::setSoundDescVolume( uint32_t _id, float _volume )
	{
		if( _id == INVALID_SOUND_ID )
		{
			return false;
		}

		if( _id >= MENGINE_MARMALADE_SOUND_MAX_COUNT )
		{
			return false;
		}

		volatile MarmaladeSoundMemoryDesc & desc = m_soundMemoryDesc[_id];

		uint8 volume_s3e = (uint8)(_volume * (S3E_SOUND_MAX_VOLUME - 1));

		desc.volume = volume_s3e;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSystem::getSoundDescPosition( uint32_t _id, float & _position )
	{
		if( _id == INVALID_SOUND_ID )
		{
			return false;
		}

		if( _id >= MENGINE_MARMALADE_SOUND_MAX_COUNT )
		{
			return false;
		}

		volatile MarmaladeSoundMemoryDesc & desc = m_soundMemoryDesc[_id];

		float position = this->carriageToPosition_( desc.carriage, desc.frequency );

		_position = position;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static uint32_t s_getAvailableDesc( volatile MarmaladeSoundMemoryDesc ** _out, volatile MarmaladeSoundMemoryDesc * _from )
	{
		uint32_t availableCount = 0;

		for( uint32_t index_desc = 0; index_desc != MENGINE_MARMALADE_SOUND_MAX_COUNT; ++index_desc )
		{
			volatile MarmaladeSoundMemoryDesc & desc = _from[index_desc];

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
		volatile MarmaladeSoundMemoryDesc * soundMemoryDesc = (volatile MarmaladeSoundMemoryDesc *)_user;

		info->m_EndSample = S3E_FALSE;

		volatile MarmaladeSoundMemoryDesc * availableDesc[MENGINE_MARMALADE_SOUND_MAX_COUNT];
		uint32_t availableCount = s_getAvailableDesc( availableDesc, soundMemoryDesc );				

		int16 * target = info->m_Target;

		uint32 numSamples = info->m_NumSamples;

		if( info->m_Mix == 0 )
		{
			memset( target, 0, numSamples * 1 * sizeof(int16) );			
		}

		if( availableCount == 0 )
		{
			return numSamples;
		}
				
		for( uint32_t sample = 0; sample != numSamples; ++sample )
		{		
			int32 origM = (int32)*(target + 0);

			for( uint32_t index = 0; index != availableCount; )
			{
				volatile MarmaladeSoundMemoryDesc * desc = availableDesc[index];

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

						++index;
					}
					else
					{
						desc->play = false;
						desc->end = true;

						--availableCount;
						availableDesc[index] = availableDesc[availableCount];
					}
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
		volatile MarmaladeSoundMemoryDesc * soundMemoryDesc = (volatile MarmaladeSoundMemoryDesc *)_user;

		info->m_EndSample = S3E_FALSE;

		volatile MarmaladeSoundMemoryDesc * availableDesc[MENGINE_MARMALADE_SOUND_MAX_COUNT];
		uint32_t availableCount = s_getAvailableDesc( availableDesc, soundMemoryDesc );		

		int16 * target = info->m_Target;

		uint32 numSamples = info->m_NumSamples;

		if( info->m_Mix == 0 )
		{
			memset( target, 0, numSamples * 2 * sizeof(int16) );			
		}

		if( availableCount == 0 )
		{
			return numSamples;
		}
		
		for( uint32_t sample = 0; sample != numSamples; ++sample )
		{		
			int32 origL = (int32)*(target + 0);
			int32 origR = (int32)*(target + 1);

			for( uint32_t index = 0; index != availableCount; )
			{
				volatile MarmaladeSoundMemoryDesc * desc = availableDesc[index];

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
					if( desc->count == -1 || (--desc->count) > 0 )
					{
						desc->carriage = 0;
						
						++index;
					}
					else
					{
						desc->play = false;
						desc->end = true;

						--availableCount;
						availableDesc[index] = availableDesc[availableCount];
					}
				}
				else
				{
					++index;
				}
			}

			*target++ = s_clipToInt16(origL);
			*target++ = s_clipToInt16(origR);
		}

		return numSamples;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSystem::initialize()
	{
		LOGGER_INFO(m_serviceProvider)( "Starting Marmalade Sound System..." );

		int32 available = s3eSoundGetInt( S3E_SOUND_AVAILABLE );

		if( available == 0 )
		{
			LOGGER_INFO(m_serviceProvider)( "Unavailable..." );

			return false;
		}

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

		m_soundOutputFrequence = s3eSoundGetInt( S3E_SOUND_OUTPUT_FREQ );

		if( m_soundOutputFrequence == -1 )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			m_soundOutputFrequence = 22050;
		}

		for( uint32_t i = 0; i != MENGINE_MARMALADE_SOUND_MAX_COUNT; ++i )
		{
			volatile MarmaladeSoundMemoryDesc & desc = m_soundMemoryDesc[i];
						
			desc.source = nullptr;
			desc.carriage = 0;
			desc.size = 0;
			desc.memory = nullptr;
			desc.frequency = 0;
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
			volatile MarmaladeSoundMemoryDesc & desc = m_soundMemoryDesc[index_desc];

			if( desc.end == false )
			{
				continue;
			}

			stdex_free( desc.memory );
			desc.memory = nullptr;
			
			desc.carriage = 0;
			desc.size = 0;
			desc.frequency = 0;
			desc.sampleStep = 0;
			desc.volume = 0;
			desc.count = 0;
			desc.pause = false;
			desc.play = false;
			desc.stop = false;
			
			MarmaladeSoundSource * source = desc.source;
			desc.source = nullptr;

			desc.end = false;

			if( source != nullptr )
			{
				source->complete();
			}
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

