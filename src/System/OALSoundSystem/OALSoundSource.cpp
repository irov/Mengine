#	include "OALSoundSource.h"

#	include "OALError.h"
#	include "OALSoundSystem.h"
#	include "OALSoundBufferBase.h"

#	include "Logger/Logger.h"

#   include <math.h>

#	define OAL_CHECK_ERROR() s_OALErrorCheck( m_serviceProvider, __FILE__, __LINE__ )

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundSource::OALSoundSource( ServiceProviderInterface * _serviceProvider, OALSoundSystem* _soundSystem )
		: m_serviceProvider(_serviceProvider)         
        , m_soundSystem(_soundSystem)
		, m_volume(1.f)
		, m_sourceId(0)
		, m_timing(0.f)
        , m_soundBuffer(NULL)
        , m_headMode(true)
        , m_playing(false)
        , m_loop(false)
	{
		m_position[0] = 0.0f;
		m_position[1] = 0.0f;
		m_position[2] = 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundSource::~OALSoundSource()
	{
		this->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::play()
	{
		if( m_playing == true || m_soundBuffer == NULL )
		{
			return;
		}

		m_sourceId = m_soundSystem->genSourceId();

		if( m_sourceId == 0 )
		{
			return;
		}

		this->apply_( m_sourceId );
		m_soundBuffer->play( m_sourceId, m_loop, m_timing );
	
		m_playing = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::pause()
	{
		if( m_playing == false || m_soundBuffer == NULL )
		{
			return;
		}

		m_playing = false;

		if( m_sourceId != 0 )
		{
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
// 			alSourceRewind( m_sourceId );
// 			OAL_CHECK_ERROR();
			m_soundSystem->releaseSourceId( sourceId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::stop()
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
	bool OALSoundSource::isPlaying() const 
	{
		return m_playing;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setVolume( float _volume )
	{
		m_volume = _volume;
        	
        if( m_playing == true && m_sourceId != 0 )
		{
			alSourcef( m_sourceId, AL_GAIN, m_volume );
			OAL_CHECK_ERROR();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSource::getVolume() const 
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setPosition( float _x, float _y, float _z )
	{
		m_position[0] = _x;
		m_position[1] = _y;
		m_position[2] = _z;

		if( m_playing == true && m_sourceId != 0 )
		{
			alSourcefv( m_sourceId, AL_POSITION, &(m_position[0]) );
			OAL_CHECK_ERROR();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const float * OALSoundSource::getPosition() const 
	{
		return &(m_position[0]);
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setLoop( bool _loop )
	{
		m_loop = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSource::getLoop() const 
	{
		return m_loop;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSource::getLengthMs() const
	{
		if( m_soundBuffer != NULL )
		{
			return m_soundBuffer->getTimeTotal() * 1000.f;
		}

		return 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSource::getPosMs() const
	{
		if( m_soundBuffer == NULL )
		{
			return 0.f;
		}

		if( m_sourceId == 0 )
		{
			return 0.f;
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
	void OALSoundSource::setPosMs( float _posMs )
	{
        m_timing = _posMs * 0.001f;	
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::loadBuffer( SoundBufferInterface* _soundBuffer )
	{
		this->unloadBuffer_();

		m_soundBuffer = static_cast<OALSoundBufferBase*>( _soundBuffer );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::unloadBuffer_()
	{
		if( m_soundBuffer != NULL && m_playing == true )
		{
			this->stop();
		}

		m_soundBuffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setHeadMode( bool _headMode )
	{
		m_headMode = _headMode;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSource::getHeadMode() const
	{
		return m_headMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::apply_( ALuint _source )
	{
		if( m_headMode )
		{
			alSourcei( _source, AL_SOURCE_RELATIVE, AL_TRUE );
			OAL_CHECK_ERROR();

			alSourcef( _source, AL_ROLLOFF_FACTOR, 0.f );
			OAL_CHECK_ERROR();

			alSource3f( _source, AL_DIRECTION, 0.f, 0.f, 0.f );
			OAL_CHECK_ERROR();
		} 
		else 
		{
			alSourcei( _source, AL_SOURCE_RELATIVE, AL_FALSE );
			OAL_CHECK_ERROR();

			alSourcef( _source, AL_ROLLOFF_FACTOR, 1.f );
			OAL_CHECK_ERROR();
		}

		alSourcei( _source, AL_LOOPING, AL_FALSE );	
		OAL_CHECK_ERROR();

		alSourcefv( _source, AL_POSITION, &(m_position[0]) );
		OAL_CHECK_ERROR();

		alSourcef( _source, AL_MIN_GAIN, 0.f );
		OAL_CHECK_ERROR();

		alSourcef( _source, AL_MAX_GAIN, 1.f );
		OAL_CHECK_ERROR();

		alSourcef( _source, AL_GAIN, m_volume );
		OAL_CHECK_ERROR();
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* OALSoundSource::getSoundBuffer() const
	{
		return m_soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
