#	include "OALSoundSource.h"

#	include "OALSoundSystem.h"
#	include "OALSoundBufferBase.h"
#	include "OALSoundError.h"

#	include "Logger/Logger.h"

#   include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundSource::OALSoundSource()
		: m_serviceProvider(nullptr)         
        , m_soundSystem(nullptr)
		, m_volume(1.f)
		, m_sourceId(0)
		, m_timing(0.f)
        , m_soundBuffer(nullptr)
        , m_headMode(true)
        , m_playing(false)
        , m_pausing(false)
        , m_loop(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundSource::~OALSoundSource()
	{
		this->releaseSourceId_();
	}
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSource::initialize( ServiceProviderInterface * _serviceProvider, OALSoundSystem * _soundSystem )
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

        if( m_pausing == false )
        {
            m_sourceId = m_soundSystem->genSourceId();

            if( m_sourceId == 0 )
            {
                return false;
            }

            this->apply_( m_sourceId );

            if( m_soundBuffer->play( m_sourceId, m_loop, m_timing ) == false )
            {
				LOGGER_ERROR(m_serviceProvider)("OALSoundSource::play invalid buffer play %d loop %d timing %f"
					, m_sourceId
					, m_loop
					, m_timing
					);

                return false;
            }
        }
        else
        {
            m_soundBuffer->resume( m_sourceId );
        }
        
        //m_timing = 0.f;
	
		m_playing = true;
        m_pausing = false;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::pause()
	{
		if( m_playing == false )
		{
			return;
		}

		if( m_pausing == true )
		{
			return;
		}

        if( m_soundBuffer == nullptr )
        {
            return;
        }
        
        if( m_sourceId == 0 )
        {
            return;
        }

		m_playing = false;
        m_pausing = true;
		 
        float timing = 0.f;
        if( m_soundBuffer->getTimePos( m_sourceId, timing ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundSource::pause invalid get time pos %d (play %d)"
                , m_sourceId
                , m_playing
                );
        }

        m_timing = timing;
		
        m_soundBuffer->pause( m_sourceId );       
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::stop()
	{
		if( m_playing == false && m_pausing == false )
		{
			return;
		}

		m_playing = false;
        m_pausing = false;

        this->releaseSourceId_();

		m_timing = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSource::isPlay() const 
	{
		return m_playing;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSource::isPause() const
	{
		return m_pausing;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setVolume( float _volume )
	{
		m_volume = _volume;
        	
        if( m_playing == true && m_sourceId != 0 )
		{
			float gain = ::powf( m_volume, 2.f );
			alSourcef( m_sourceId, AL_GAIN, gain );
			OAL_CHECK_ERROR(m_serviceProvider);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSource::getVolume() const 
	{
		return m_volume;
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
	float OALSoundSource::getDuration() const
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
	float OALSoundSource::getPosition() const
	{
		if( m_soundBuffer == nullptr )
		{
			return 0.f;
		}

		if( m_sourceId == 0 )
		{
			return m_timing;
		}

		if( m_pausing == true )
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
	bool OALSoundSource::setPosition( float _posMs )
	{
		if( m_playing == true )
		{
			return false;
		}

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

		//if( m_playing == true || m_pausing == true )
		//{
		//	if( m_soundBuffer->setTimePos( m_sourceId, posmc ) == false )
		//	{
		//		LOGGER_ERROR(m_serviceProvider)("OALSoundSource::setPosMs invalid set time pos %d time %f (play %d)"
		//			, m_sourceId
		//			, posmc
		//			, m_playing
		//			);

		//		return false;
		//	}
		//}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
	{
		this->unloadBuffer_();

		m_soundBuffer = stdex::intrusive_static_cast<OALSoundBufferBasePtr>(_soundBuffer);
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
    void OALSoundSource::releaseSourceId_()
    {
        if( m_sourceId != 0 )
        {
            ALuint sourceId = m_sourceId;
            m_sourceId = 0;

            m_soundBuffer->stop( sourceId );
            m_soundSystem->releaseSourceId( sourceId );
        }
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
		if( m_headMode == true )
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

            alSource3f( _source, AL_DIRECTION, 0.f, 0.f, 0.f );
            OAL_CHECK_ERROR(m_serviceProvider);
		}

		alSourcei( _source, AL_LOOPING, AL_FALSE );	
		OAL_CHECK_ERROR(m_serviceProvider);

		alSource3f( _source, AL_POSITION, 0.f, 0.f, 0.f );
		OAL_CHECK_ERROR(m_serviceProvider);

        alSource3f( _source, AL_VELOCITY, 0.f, 0.f, 0.f);
        OAL_CHECK_ERROR(m_serviceProvider);

		alSourcef( _source, AL_MIN_GAIN, 0.f );
		OAL_CHECK_ERROR(m_serviceProvider);

		alSourcef( _source, AL_MAX_GAIN, 1.f );
		OAL_CHECK_ERROR(m_serviceProvider);

        alSourcef( _source, AL_PITCH, 1.f);
        OAL_CHECK_ERROR(m_serviceProvider);

		float gain = ::powf( m_volume, 2.f );
		alSourcef( _source, AL_GAIN, gain );
		OAL_CHECK_ERROR(m_serviceProvider);
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr OALSoundSource::getSoundBuffer() const
	{
		return m_soundBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
