#include "OpenALSoundSource.h"

#include "OpenALSoundSystem.h"
#include "OpenALSoundBufferBase.h"
#include "OpenALSoundError.h"

#include "Kernel/Logger.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundSource::OpenALSoundSource()
        : m_soundSystem( nullptr )
        , m_volume( 1.f )
        , m_sourceId( 0 )
        , m_timing( 0.f )
        , m_headMode( true )
        , m_playing( false )
        , m_pausing( false )
        , m_loop( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundSource::~OpenALSoundSource()
    {
        this->releaseSourceId_();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::setSoundSystem( OpenALSoundSystem * _soundSystem )
    {
        m_soundSystem = _soundSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundSystem* OpenALSoundSource::getSoundSystem() const
    {
        return m_soundSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSource::play()
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

            if( m_soundBuffer->playSource( m_sourceId, m_loop, m_timing ) == false )
            {
                LOGGER_ERROR( "invalid buffer play %d loop %d timing %f"
                    , m_sourceId
                    , m_loop
                    , m_timing
                );

                return false;
            }
        }
        else
        {
            m_soundBuffer->resumeSource( m_sourceId );
        }

        //m_timing = 0.f;

        m_playing = true;
        m_pausing = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::pause()
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

        m_playing = true;
        m_pausing = true;

        m_soundBuffer->pauseSource( m_sourceId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSource::resume()
    {
        if( m_playing == false )
        {
            return false;
        }

        if( m_soundBuffer == nullptr )
        {
            return false;
        }

        if( m_pausing == false )
        {
            return false;
        }

        m_soundBuffer->resumeSource( m_sourceId );
        
        m_playing = true;
        m_pausing = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::stop()
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
    bool OpenALSoundSource::isPlay() const
    {
        return m_playing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSource::isPause() const
    {
        return m_pausing;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::setVolume( float _volume )
    {
        m_volume = _volume;

        if( m_playing == true && m_sourceId != 0 )
        {
            ALfloat gain = ::powf( m_volume, 2.f );
            OPENAL_CALL( alSourcef, (m_sourceId, AL_GAIN, gain) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenALSoundSource::getVolume() const
    {
        return m_volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSource::getLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenALSoundSource::getDuration() const
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ERROR( "invalid sound buffer"
            );

            return 0.f;
        }

        float time_sound = m_soundBuffer->getTimeTotal();

        return time_sound;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenALSoundSource::getPosition() const
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
            LOGGER_ERROR( "invalid get time pos %d (play %d)"
                , m_sourceId
                , m_playing
            );

            return 0.f;
        }

        //timing dont assign to zero when m_soundBuffer is stopped!
        if( fabsf( posms ) < 0.0001f && fabsf( m_timing ) > 0.0001f )
        {
            posms = m_timing;
        }

        return posms;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSource::setPosition( float _posMs )
    {
        if( m_playing == true )
        {
            return false;
        }

        float posmc = _posMs;

        float total = m_soundBuffer->getTimeTotal();

        if( posmc > total )
        {
            LOGGER_ERROR( "pos %f total %f"
                , posmc
                , total
            );

            return false;
        }

        if( posmc < 0.f )
        {
            LOGGER_ERROR( "pos %f less zero"
                , posmc
            );

            posmc = 0.f;
        }

        m_timing = posmc;

        //if( m_playing == true || m_pausing == true )
        //{
        //	if( m_soundBuffer->setTimePos( m_sourceId, posmc ) == false )
        //	{
        //		LOGGER_ERROR("invalid set time pos %d time %f (play %d)"
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
    void OpenALSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->unloadBuffer_();

        m_soundBuffer = stdex::intrusive_static_cast<OpenALSoundBufferBasePtr>(_soundBuffer);
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::unloadBuffer_()
    {
        if( m_soundBuffer != nullptr && m_playing == true )
        {
            this->stop();
        }

        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::releaseSourceId_()
    {
        if( m_sourceId != 0 )
        {
            ALuint sourceId = m_sourceId;
            m_sourceId = 0;

            m_soundBuffer->stopSource( sourceId );
            m_soundSystem->releaseSourceId( sourceId );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::setHeadMode( bool _headMode )
    {
        m_headMode = _headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSource::getHeadMode() const
    {
        return m_headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::apply_( ALuint _source )
    {
        if( m_headMode == true )
        {
            OPENAL_CALL( alSourcei, (_source, AL_SOURCE_RELATIVE, AL_TRUE) );
            OPENAL_CALL( alSourcef, (_source, AL_ROLLOFF_FACTOR, 0.f) );
            OPENAL_CALL( alSource3f, (_source, AL_DIRECTION, 0.f, 0.f, 0.f) );
        }
        else
        {
            OPENAL_CALL( alSourcei, ( _source, AL_SOURCE_RELATIVE, AL_FALSE ));
            OPENAL_CALL( alSourcef, (_source, AL_ROLLOFF_FACTOR, 1.f) );
            OPENAL_CALL( alSource3f, (_source, AL_DIRECTION, 0.f, 0.f, 0.f) );
        }

        OPENAL_CALL( alSourcei, (_source, AL_LOOPING, AL_FALSE) );
        OPENAL_CALL( alSource3f, (_source, AL_POSITION, 0.f, 0.f, 0.f) );
        OPENAL_CALL( alSource3f, (_source, AL_VELOCITY, 0.f, 0.f, 0.f) );
        OPENAL_CALL( alSourcef, (_source, AL_MIN_GAIN, 0.f) );
        OPENAL_CALL( alSourcef, (_source, AL_MAX_GAIN, 1.f) );
        OPENAL_CALL( alSourcef, (_source, AL_PITCH, 1.f) );

        ALfloat gain = ::powf( m_volume, 2.f );
        OPENAL_CALL( alSourcef, (_source, AL_GAIN, gain) );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & OpenALSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
}
