#	include "MarmaladeSoundSource.h"

#	include "MarmaladeSoundSystem.h"

#	include "MarmaladeSoundError.h"

#	include "Logger/Logger.h"

#	include "stdex/allocator.h"

#	include "s3eDevice.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct SoundMemoryDesc
	{
		uint32 carriage;
		int16 * memory;		
		bool destroy;
	};
	//////////////////////////////////////////////////////////////////////////
	volatile static SoundMemoryDesc s_soundMemoryDesc[64];
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSource::MarmaladeSoundSource()
		: m_serviceProvider(nullptr)
		, m_soundSystem(nullptr)
		, m_volume(1.f)
		, m_playing(false)
		, m_pausing(false)
        , m_loop(false)
		, m_soundChannel(-1)
		, m_volume_s3e(255)
		, m_stereo_s3e(true)
		, m_carriage_s3e(0)
		, m_W(0)
		, m_L(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSource::~MarmaladeSoundSource()
	{
		if( m_playing == true || m_pausing == true )
		{
			this->stop();
		}

		this->checkMemoryCache_();
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeSoundSource::initialize( ServiceProviderInterface * _serviceProvider, MarmaladeSoundSystem * _soundSystem )
    {
        m_serviceProvider = _serviceProvider;
		m_soundSystem = _soundSystem;				
    }
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeSoundSource::s_AudioCallback( void * _sys, void * _user )
	{
		s3eSoundGenAudioInfo * info = (s3eSoundGenAudioInfo *)_sys;
		MarmaladeSoundSource * source = (MarmaladeSoundSource *)_user;

		int32 samplesPlayed = source->audioCallback( info );

		return samplesPlayed;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeSoundSource::s_EndSampleCallback( void * _sys, void * _user )
	{
		s3eSoundEndSampleInfo * info = (s3eSoundEndSampleInfo*)_sys;
		MarmaladeSoundSource * source = (MarmaladeSoundSource *)_user;

		source->endSampleCallback( info );

		return info->m_RepsRemaining;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeSoundSource::s_StopAudioCallback( void * _sys, void * _user )
	{
		s3eSoundEndSampleInfo * info = (s3eSoundEndSampleInfo*)_sys;
		MarmaladeSoundSource * source = (MarmaladeSoundSource *)_user;

		source->stopAudioCallback( info );

		return info->m_RepsRemaining;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeSoundSource::audioCallback( s3eSoundGenAudioInfo * _info )
	{
		volatile SoundMemoryDesc & desc = s_soundMemoryDesc[_info->m_Channel];

		_info->m_EndSample = 0;

		int16 * target = _info->m_Target;

		uint32 inputSampleSize = m_stereo_s3e == true ? 2 : 1;

		int32 samplesPlayed = 0;

		// For stereo output, info->m_NumSamples is number of l/r pairs (each sample is 32bit)
		// info->m_OrigNumSamples always measures the total number of 16 bit samples,
		// regardless of whether input was mono or stereo.

		uint32 outputSampleSize = _info->m_Stereo ? 2 : 1;
		uint32 numSamples = _info->m_NumSamples;			
		
		// Loop through samples (mono) or sample-pairs (stereo) required.
		// If stereo, we copy the 16bit sample for each l/r channel and do per
		// left/right channel processing on each sample for the pair. i needs
		// scaling when comparing to input sample count as that is always 16bit.
		for( uint32 i = 0; i != numSamples; ++i )
		{
			// For each sample (pair) required, we either do:
			//  * get mono sample if input is mono (output can be either)
			//  * get left sample if input is stereo (output can be either)
			//  * get right sample if input and output are both stereo

			uint32 outPosLeft = ((desc.carriage + i) * m_W / m_L) * inputSampleSize;

			// Stop when hitting end of data. Must scale to 16bit if stereo
			// (m_OrigNumSamples is always 16bit) and by resample factor as we're
			// looping through output position, not input.
			if( outPosLeft >= _info->m_OrigNumSamples )
			{	
				_info->m_EndSample = 1;

				if( _info->m_Mix == 0 )
				{
					std::memset( target, 0, (numSamples - i) * outputSampleSize * sizeof(int16) );
				}

				break;
			}

			int16 yLeft = (_info->m_OrigStart[outPosLeft + 0] * m_volume_s3e) >> 8;
			int16 yRight = (_info->m_OrigStart[outPosLeft + 1] * m_volume_s3e) >> 8;
	
			////apply FIR filter to output to remove artifacts
			//m_filterBufferPos = (m_filterBufferPos + 1) % MARMALADE_SOUND_NUM_COEFFICIENTS;

			//// use circular buffer to store previous inputs
			//m_filterBufferL[m_filterBufferPos] = yLeft;

			//yLeft = 0;
			//for( int k = 0; k != MARMALADE_SOUND_NUM_COEFFICIENTS; ++k )
			//{
			//	yLeft += (int16)(m_filterCoefficients[k] * m_filterBufferL[(m_filterBufferPos + k) % MARMALADE_SOUND_NUM_COEFFICIENTS]);
			//}

			//m_filterBufferR[m_filterBufferPos] = yRight;
			//
			//yRight = 0;
			//for( int k = 0; k != MARMALADE_SOUND_NUM_COEFFICIENTS; ++k )
			//{
			//	yRight += (int16)(m_filterCoefficients[k] * m_filterBufferR[(m_filterBufferPos + k) % MARMALADE_SOUND_NUM_COEFFICIENTS]);
			//}


			int16 origL = 0;
			int16 origR = 0;
			if( _info->m_Mix != 0 )
			{
				origL = *target;
				origR = *(target+1);
			}

			*target++ = s_clipToInt16(yLeft + origL);
			*target++ = s_clipToInt16(yRight + origR);
			
			samplesPlayed++;
		}

		// Update global output pointer (track samples played in app)
		desc.carriage += samplesPlayed;

		// Inform s3e sound how many samples we played
		return samplesPlayed;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::endSampleCallback( s3eSoundEndSampleInfo * _info )
	{
		if( _info->m_RepsRemaining == 0 )
		{
			s_soundMemoryDesc[_info->m_Channel].destroy = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::stopAudioCallback( s3eSoundEndSampleInfo * _info )
	{
		if( _info->m_RepsRemaining == 0 )
		{
			s_soundMemoryDesc[_info->m_Channel].destroy = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::play()
	{
		if( m_playing == true )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::play alredy play"
				);

			return false;
		}

		if( m_pausing == true )
		{
			uint32 carriage = s_soundMemoryDesc[m_soundChannel].carriage;

			if( m_carriage_s3e != carriage )
			{
				s_soundMemoryDesc[m_soundChannel].carriage = carriage;
			}

			if( s3eSoundChannelResume( m_soundChannel ) == S3E_RESULT_ERROR )
			{
				MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

				return false;
			}

			m_pausing = false;

			return true;
		}

		int soundChannel = s3eSoundGetFreeChannel();

		if( soundChannel == -1 )
		{
			return false;
		}

		if( s3eSoundChannelRegister( soundChannel, S3E_CHANNEL_GEN_AUDIO, &s_AudioCallback, this ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			return false;
		}

		if( s3eSoundChannelRegister( soundChannel, S3E_CHANNEL_END_SAMPLE, &s_EndSampleCallback, this ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			return false;
		}

		if( s3eSoundChannelRegister( soundChannel, S3E_CHANNEL_STOP_AUDIO, &s_EndSampleCallback, this ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			return false;
		}

		if( m_soundSystem->isDeviceStereo() == true )
		{
			if( s3eSoundChannelRegister( soundChannel, S3E_CHANNEL_GEN_AUDIO_STEREO, &s_AudioCallback, this ) == S3E_RESULT_ERROR )
			{
				MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

				return false;
			}
		}
				
		m_filterBufferPos = -1;

		const SoundDecoderInterfacePtr & decoder = m_soundBuffer->getDecoder();

		const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

		this->checkMemoryCache_();

		int16 * memory_s3e = (int16 *)stdex_malloc( dataInfo->size );

		decoder->decode( memory_s3e, dataInfo->size );

		if( s_soundMemoryDesc[soundChannel].memory != nullptr )
		{
 			return false;
		}

		s_soundMemoryDesc[soundChannel].carriage = m_carriage_s3e;
		s_soundMemoryDesc[soundChannel].memory = memory_s3e;
		s_soundMemoryDesc[soundChannel].destroy = false;

		if( s3eSoundChannelPlay( soundChannel, memory_s3e, m_soundBuffer->getSamples(), (m_loop == true ? 0 : 1), 0 ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			stdex_free( memory_s3e );

			return false;
		}

		LOGGER_ERROR(m_serviceProvider)("play channel!!!!!!!!!! %d"
			, soundChannel
			);
		
		m_playing = true;
		m_pausing = false;

		m_soundChannel = soundChannel;
		
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::pause()
	{
		if( m_playing == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::pause invalid 'not playing'"
				);

			return;
		}

		if( m_pausing == true )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::pause invalid 'alredy pausing'"
				);

			return;
		}

		if( s3eSoundChannelPause( m_soundChannel ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}

		m_carriage_s3e = s_soundMemoryDesc[m_soundChannel].carriage;

		m_playing = false;
		m_pausing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::stop()
	{
		if( m_playing == false && m_pausing == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::stop invalid 'not playing or pausing'"
				);

			return;
		}

		if( s3eSoundChannelStop( m_soundChannel ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}
		
		if( m_soundSystem->isDeviceStereo() == true )
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

		if( s3eSoundChannelUnRegister( m_soundChannel, S3E_CHANNEL_END_SAMPLE ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}


		if( s3eSoundChannelUnRegister( soundChannel, S3E_CHANNEL_STOP_AUDIO ) == S3E_RESULT_ERROR )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}

		m_soundChannel = -1;
		m_carriage_s3e = 0;
		
		m_playing = false;
		m_pausing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::isPlaying() const 
	{
		return m_playing;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::setVolume( float _volume )
	{
		m_volume = _volume;

		m_volume_s3e = (uint8)(m_volume * S3E_SOUND_MAX_VOLUME);
	}
	//////////////////////////////////////////////////////////////////////////
	float MarmaladeSoundSource::getVolume() const 
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::setLoop( bool _loop )
	{
		m_loop = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::getLoop() const 
	{
		return m_loop;
	}
	//////////////////////////////////////////////////////////////////////////
	float MarmaladeSoundSource::getLengthMs() const
	{
		if( m_soundBuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::getLengthMs invalid sound buffer"
				);

			return 0.f;
		}

		float time_sound = m_soundBuffer->getLength();

		return time_sound;
	}
	//////////////////////////////////////////////////////////////////////////
	float MarmaladeSoundSource::getPosMs() const
	{
		uint32 carriage = (m_playing == false || m_pausing == true) 
			? m_carriage_s3e 
			: s_soundMemoryDesc[m_soundChannel].carriage;

		uint32 pos = carriage * m_L / m_W;

		uint32_t frequency = m_soundBuffer->getFrequency();
		uint32_t channels = m_soundBuffer->getChannels();
		uint32_t bits = m_soundBuffer->getBits();

		uint32_t bytePerSec = (frequency * channels * bits);

		float posMs = (float)(pos * 1000 / bytePerSec);

		return posMs;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::setPosMs( float _posMs )
	{
		if( m_playing == true )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::setPosMs invalid setup pos if playing! [%f]"
				, _posMs
				);

			return false;
		}

		uint32 ms = (uint32)_posMs;

		uint32_t frequency = m_soundBuffer->getFrequency();
		uint32_t channels = m_soundBuffer->getChannels();
		uint32_t bits = m_soundBuffer->getBits();

		uint32_t pos = ms * (frequency * channels * bits) / 1000;

		m_carriage_s3e = pos * m_W / m_L;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
	{
		m_soundBuffer = stdex::intrusive_static_cast<MarmaladeSoundBufferPtr>(_soundBuffer);

		m_stereo_s3e = m_soundBuffer->getStereo();

		int32 inputFrequency = m_soundBuffer->getFrequency();
		int32 outputFrequency = s3eSoundGetInt( S3E_SOUND_OUTPUT_FREQ );		

		int32 gcd = s_GCD( inputFrequency, outputFrequency );
		m_W	= inputFrequency / gcd;
		m_L	= outputFrequency / gcd;

		//std::memset( m_filterBufferL, 0, sizeof(int16) * MARMALADE_SOUND_NUM_COEFFICIENTS );
		//std::memset( m_filterBufferR, 0, sizeof(int16) * MARMALADE_SOUND_NUM_COEFFICIENTS );
		//s_setupFilterCoefficients( m_filterCoefficients, inputFrequency, outputFrequency );
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr MarmaladeSoundSource::getSoundBuffer() const
	{
		return m_soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::checkMemoryCache_()
	{
		for( uint32_t i = 0; i != 64; ++i )
		{
			volatile SoundMemoryDesc & cache = s_soundMemoryDesc[i];

			if( cache.destroy == true )
			{
				stdex_free( cache.memory );

				cache.memory = nullptr;
				cache.destroy = false;
			}
		}
	}
}	// namespace Menge
