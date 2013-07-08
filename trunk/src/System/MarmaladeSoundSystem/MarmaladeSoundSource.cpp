#	include "MarmaladeSoundSource.h"

#	include "MarmaladeError.h"
#	include "MarmaladeSoundSystem.h"
#	include "MarmaladeSoundBufferBase.h"

#	include "Logger/Logger.h"

#   include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSource::MarmaladeSoundSource()
		: m_serviceProvider(nullptr)         
        , m_soundSystem(nullptr)
		, m_volume(1.f)
		, m_sourceId(0)
		, m_timing(0.f)
        , m_soundBuffer(nullptr)
        , m_headMode(true)
        , m_playing(false)
        , m_loop(false)
	{
		m_position[0] = 0.0f;
		m_position[1] = 0.0f;
		m_position[2] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSource::~MarmaladeSoundSource()
	{
		this->stop();
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeSoundSource::initialize( ServiceProviderInterface * _serviceProvider, MarmaladeSoundSystem* _soundSystem )
    {
        m_serviceProvider = _serviceProvider;
        m_soundSystem = _soundSystem;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::play()
	{
		if( m_playing == true || m_soundBuffer == NULL )
		{
			return true;
		}

		m_sourceId = m_soundSystem->genSourceId();

		if( m_sourceId == 0 )
		{
			return false;
		}

		this->apply_( m_sourceId );
		m_soundBuffer->play( m_sourceId, m_loop, m_timing );
	
		m_playing = true;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::pause()
	{
		if( m_playing == false || m_soundBuffer == NULL )
		{
			return;
		}

		m_playing = false;

		if( m_sourceId == 0 )
		{
            return;
        }
         
        float timing = 0.f;
        if( m_soundBuffer->getTimePos( m_sourceId, timing ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundSource::pause invalid get time pos %d (play %d)"
                , m_sourceId
                , m_playing
                );
        }

        m_timing = timing;

        ALuint sourceId = m_sourceId;
        m_sourceId = 0;

        m_soundBuffer->stop( sourceId );
        m_soundSystem->releaseSourceId( sourceId );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::stop()
	{
		if( m_playing == false )
		{
			return;
		}

		m_playing = false;

		if( m_sourceId != 0 )
		{
            ALuint sourceId = m_sourceId;
            m_sourceId = 0;

			m_soundBuffer->stop( sourceId );
// 			alSourceRewind( m_sourceId );
// 			OAL_CHECK_ERROR();
			m_soundSystem->releaseSourceId( sourceId );
		}

		m_timing = 0.f;
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
        	
        if( m_playing == true && m_sourceId != 0 )
		{
			alSourcef( m_sourceId, AL_GAIN, m_volume );
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float MarmaladeSoundSource::getVolume() const 
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::setPosition( const mt::vec3f & _pos )
	{
		m_position = _pos;

		if( m_playing == true && m_sourceId != 0 )
		{
			alSourcefv( m_sourceId, AL_POSITION, m_position.buff() );
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & MarmaladeSoundSource::getPosition() const 
	{
		return m_position;
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
		if( m_soundBuffer != NULL )
		{
			return m_soundBuffer->getTimeTotal() * 1000.f;
		}

		return 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	float MarmaladeSoundSource::getPosMs() const
	{
		if( m_soundBuffer == NULL )
		{
			return 0.f;
		}

		if( m_sourceId == 0 )
		{
			return m_timing * 1000.f;
		}
			
		float posms = 0.f;
        if( m_soundBuffer->getTimePos( m_sourceId, posms ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundSource::getPosMs invalid get time pos %d (play %d)"
                , m_sourceId
                , m_playing
                );

            return 0.f;
        }
		
		//timing dont assign to zero when m_soundBuffer is stopped!
		if( fabsf(posms) < 0.0001f && fabsf(m_timing) > 0.0001f )
		{ 
			posms = m_timing;
		}

		posms *= 1000.f;
		
		return posms;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::setPosMs( float _posMs )
	{
        m_timing = _posMs * 0.001f;	

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::setSoundBuffer( SoundBufferInterface* _soundBuffer )
	{
		this->unloadBuffer_();

		m_soundBuffer = static_cast<MarmaladeSoundBufferBase*>( _soundBuffer );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::unloadBuffer_()
	{
		if( m_soundBuffer != NULL && m_playing == true )
		{
			this->stop();
		}

		m_soundBuffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::setHeadMode( bool _headMode )
	{
		m_headMode = _headMode;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSource::getHeadMode() const
	{
		return m_headMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSource::apply_( ALuint _source )
	{
		if( m_headMode )
		{
			alSourcei( _source, AL_SOURCE_RELATIVE, AL_TRUE );
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			alSourcef( _source, AL_ROLLOFF_FACTOR, 0.f );
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			alSource3f( _source, AL_DIRECTION, 0.f, 0.f, 0.f );
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		} 
		else 
		{
			alSourcei( _source, AL_SOURCE_RELATIVE, AL_FALSE );
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			alSourcef( _source, AL_ROLLOFF_FACTOR, 1.f );
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
		}

		alSourcei( _source, AL_LOOPING, AL_FALSE );	
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcefv( _source, AL_POSITION, &(m_position[0]) );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcef( _source, AL_MIN_GAIN, 0.f );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcef( _source, AL_MAX_GAIN, 1.f );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

		alSourcef( _source, AL_GAIN, m_volume );
		MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* MarmaladeSoundSource::getSoundBuffer() const
	{
		return m_soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
