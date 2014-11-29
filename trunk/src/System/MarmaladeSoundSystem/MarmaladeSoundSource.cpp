#	include "MarmaladeSoundSource.h"

#	include "MarmaladeSoundSystem.h"

#	include "MarmaladeSoundError.h"

#	include "Logger/Logger.h"

#	include "stdex/allocator.h"

#	include "s3eDevice.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSource::MarmaladeSoundSource()
		: m_serviceProvider(nullptr)
		, m_soundSystem(nullptr)
		, m_volume(1.f)
		, m_playing(false)
		, m_pausing(false)
        , m_loop(false)
		, m_volume_s3e(255)
		, m_stereo_s3e(true)
		, m_carriage_s3e(0)
		, m_soundId((uint32_t)-1)
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
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeSoundSource::initialize( ServiceProviderInterface * _serviceProvider, MarmaladeSoundSystem * _soundSystem )
    {
        m_serviceProvider = _serviceProvider;
		m_soundSystem = _soundSystem;				
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
			volatile SoundMemoryDesc * desc = m_soundSystem->getSound( m_soundId );

			uint32 carriage = desc->carriage;

			if( m_carriage_s3e != carriage )
			{
				desc->carriage = carriage;
			}

			desc->pause = false;

			m_pausing = false;

			return true;
		}

		m_soundId = m_soundSystem->newSound( this );

		if( m_soundId == (uint32_t)-1 )
		{
			return false;
		}
				
		const SoundDecoderInterfacePtr & decoder = m_soundBuffer->getDecoder();

		if( decoder->rewind() == false )
		{
			return false;
		}

		const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

		int16 * memory_s3e = (int16 *)stdex_malloc( dataInfo->size );

		size_t size = decoder->decode( memory_s3e, dataInfo->size );

		if( size != dataInfo->size )
		{
			return false;
		}

		m_playing = true;
		m_pausing = false;

		volatile SoundMemoryDesc * desc = m_soundSystem->getSound( m_soundId );
					
		desc->carriage = m_carriage_s3e;
		desc->size = (((uint32_t)dataInfo->length) * dataInfo->frequency) / 1000 * m_W / m_L;
		desc->memory = memory_s3e;

		uint32_t sampleSize = (dataInfo->stereo == true ? 2 : 1);
		desc->sampleStep = sampleSize;
		desc->volume = m_volume_s3e;

		desc->pause = false;
		desc->stop = false;
		desc->end = false;

		desc->count = m_loop == true ? -1 : 1;

		desc->play = true;
		
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

		volatile SoundMemoryDesc * desc = m_soundSystem->getSound( m_soundId );

		desc->pause = true;

		m_carriage_s3e = desc->carriage;

		m_playing = false;
		m_pausing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::stop()
	{
		if( m_playing == false && m_pausing == false )
		{
			return;
		}

		volatile SoundMemoryDesc * desc = m_soundSystem->getSound( m_soundId );

		desc->stop = true;
		
		m_carriage_s3e = 0;
		
		m_playing = false;
		m_pausing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::complete()
	{
		m_soundId = 0;
		m_carriage_s3e = 0;

		m_playing = false;
		m_pausing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::isPlay() const 
	{
		return m_playing;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::isPause() const
	{
		return m_pausing;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::setVolume( float _volume )
	{
		m_volume = _volume;

		m_volume_s3e = (uint8)(m_volume * S3E_SOUND_MAX_VOLUME);

		if( m_playing == true )
		{
			volatile SoundMemoryDesc * desc = m_soundSystem->getSound( m_soundId );

			desc->volume = m_volume_s3e;
		}
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
			: m_soundSystem->getSound( m_soundId )->carriage;

		uint32 pos = carriage;

		uint32_t frequency = m_soundBuffer->getFrequency();
		uint32_t channels = m_soundBuffer->getChannels();
		//uint32_t bits = m_soundBuffer->getBits();

		//uint32_t bytePerSec = (frequency * channels * bits);

		float posMs = (float)(pos * 1000 / frequency * m_L / m_W);

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
		//uint32_t bits = m_soundBuffer->getBits();

		uint32_t pos = (ms * frequency) / 1000 * m_W / m_L;

		//m_carriage_s3e = pos * m_W / m_L;
		m_carriage_s3e = pos;

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
}	// namespace Menge
