#	include "OALSoundSource.h"

#	include "OALError.h"
#	include "OALSoundSystem.h"
#	include "OALSoundBufferBase.h"

#	include "Logger/Logger.h"

#   include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundSource::OALSoundSource()
		: m_serviceProvider(nullptr)         
        , m_soundSystem(nullptr)
        , m_position(0.f, 0.f, 0.f)
		, m_volume(1.f)
		, m_sourceId(0)
		, m_timing(0.f)
        , m_soundBuffer(nullptr)
        , m_headMode(true)
        , m_playing(false)
        , m_loop(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundSource::~OALSoundSource()
	{
		this->stop();
	}
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSource::initialize( ServiceProviderInterface * _serviceProvider, OALSoundSystem* _soundSystem )
    {
        m_serviceProvider = _serviceProvider;
        m_soundSystem = _soundSystem;
    }
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSource::play()
	{
        if( m_playing == true )
        {
            return true;
        }

		if( m_soundBuffer == nullptr )
		{
			return false;
		}

		m_sourceId = m_soundSystem->genSourceId();

		if( m_sourceId == 0 )
		{
			return false;
		}

		this->apply_( m_sourceId );
		
        if( m_soundBuffer->play( m_sourceId, m_loop, m_timing ) == false )
        {
            return false;
        }

        //m_timing = 0.f;
	
		m_playing = true;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::pause()
	{
		if( m_playing == false || m_soundBuffer == nullptr )
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
			OAL_CHECK_ERROR(m_serviceProvider);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSource::getVolume() const 
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
    void OALSoundSource::setPosition( const mt::vec3f & _pos )
	{
		m_position = _pos;

		if( m_playing == true && m_sourceId != 0 )
		{
			alSourcefv( m_sourceId, AL_POSITION, _pos.buff() );
			OAL_CHECK_ERROR(m_serviceProvider);
		}
	}
	//////////////////////////////////////////////////////////////////////////
    const mt::vec3f & OALSoundSource::getPosition() const 
	{
		return m_position;
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
		if( m_soundBuffer == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)("OALSoundSource::getLengthMs invalid sound buffer"
                );

            return 0.f;
        }

        float time_sound = m_soundBuffer->getTimeTotal();

		return time_sound;
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
			return m_timing;
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
        		
		return posms;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSource::setPosMs( float _posMs )
	{
        float posmc = _posMs;

        float total = m_soundBuffer->getTimeTotal();

        if( posmc > total )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundSource::setPosMs pos %f total %f"
                , _posMs
                , total
                );

            return false;
        }

        m_timing = posmc;	

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setSoundBuffer( SoundBufferInterface* _soundBuffer )
	{
		this->unloadBuffer_();

		m_soundBuffer = static_cast<OALSoundBufferBase*>( _soundBuffer );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::unloadBuffer_()
	{
		if( m_soundBuffer != nullptr && m_playing == true )
		{
			this->stop();
		}

		m_soundBuffer = nullptr;
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
			OAL_CHECK_ERROR(m_serviceProvider);

			alSourcef( _source, AL_ROLLOFF_FACTOR, 0.f );
			OAL_CHECK_ERROR(m_serviceProvider);

			alSource3f( _source, AL_DIRECTION, 0.f, 0.f, 0.f );
			OAL_CHECK_ERROR(m_serviceProvider);
		} 
		else 
		{
			alSourcei( _source, AL_SOURCE_RELATIVE, AL_FALSE );
			OAL_CHECK_ERROR(m_serviceProvider);

			alSourcef( _source, AL_ROLLOFF_FACTOR, 1.f );
			OAL_CHECK_ERROR(m_serviceProvider);
		}

		alSourcei( _source, AL_LOOPING, AL_FALSE );	
		OAL_CHECK_ERROR(m_serviceProvider);

		alSourcefv( _source, AL_POSITION, m_position.buff() );
		OAL_CHECK_ERROR(m_serviceProvider);

		alSourcef( _source, AL_MIN_GAIN, 0.f );
		OAL_CHECK_ERROR(m_serviceProvider);

		alSourcef( _source, AL_MAX_GAIN, 1.f );
		OAL_CHECK_ERROR(m_serviceProvider);

		alSourcef( _source, AL_GAIN, m_volume );
		OAL_CHECK_ERROR(m_serviceProvider);
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* OALSoundSource::getSoundBuffer() const
	{
		return m_soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
