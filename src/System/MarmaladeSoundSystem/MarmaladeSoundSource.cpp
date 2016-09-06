#	include "MarmaladeSoundSource.h"

#	include "MarmaladeSoundSystem.h"

#	include "MarmaladeSoundError.h"

#	include "Logger/Logger.h"

#	include "stdex/allocator.h"

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
		, m_position(0.f)
		, m_soundId(INVALID_SOUND_ID)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSource::~MarmaladeSoundSource()
	{
		m_soundSystem->removeSoundDesc( m_soundId );

		m_soundId = INVALID_SOUND_ID;
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
			if( m_soundSystem->resumeSoundDesc( m_soundId, m_position ) == false )
			{
				return false;
			}

			m_pausing = false;

			return true;
		}
		
		m_soundId = m_soundSystem->playSoundDesc( this, m_position, m_volume, m_loop == true ? -1 : 1 );

		if( m_soundId == INVALID_SOUND_ID )
		{
			return false;
		}

		m_playing = true;
		m_pausing = false;
		
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

		if( m_soundSystem->pauseSoundDesc( m_soundId, m_position ) == false )
		{
			return;
		}

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

		if( m_soundSystem->stopSoundDesc( m_soundId ) == false )
		{
			return;
		}
		
		m_position = 0.f;
		
		m_playing = false;
		m_pausing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::complete()
	{
		m_soundId = INVALID_SOUND_ID;
		m_position = 0.f;

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
		
		if( m_playing == false )
		{
			return;
		}
		
		m_soundSystem->setSoundDescVolume( m_soundId, _volume );
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
	float MarmaladeSoundSource::getDuration() const
	{
		if( m_soundBuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::getLengthMs invalid sound buffer"
				);

			return 0.f;
		}

		const SoundDecoderInterfacePtr & decoder = m_soundBuffer->getDecoder();

		if( decoder == nullptr )
		{
			return 0.f;
		}

		const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

		float time_sound = dataInfo->length;

		return time_sound;
	}
	//////////////////////////////////////////////////////////////////////////
	float MarmaladeSoundSource::getPosition() const
	{
		if( m_playing == false )
		{
			return 0.f;
		}

		if( m_pausing == true )
		{
			return m_position;
		}

		float posMs = 0.f;

		if( m_soundSystem->getSoundDescPosition( m_soundId, posMs ) == false )
		{
			return 0.f;
		}

		return posMs;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::setPosition( float _posMs )
	{
		if( m_playing == true )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::setPosMs invalid setup pos if playing! [%f]"
				, _posMs
				);

			return false;
		}

		m_position = _posMs;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
	{
		m_soundBuffer = _soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr MarmaladeSoundSource::getSoundBuffer() const
	{
		return m_soundBuffer;
	}
}	// namespace Menge
