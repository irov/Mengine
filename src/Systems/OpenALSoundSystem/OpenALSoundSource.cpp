#include "OpenALSoundSource.h"

#include "OpenALSoundSystemExtensionInterface.h"
#include "OpenALSoundBufferBase.h"
#include "OpenALSoundErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundSource::OpenALSoundSource()
        : m_volume( 1.f )
        , m_sourceId( 0 )
        , m_time( 0.f )
        , m_headMode( true )
        , m_playing( false )
        , m_pausing( false )
        , m_loop( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundSource::~OpenALSoundSource()
    {
        MENGINE_ASSERTION_FATAL( m_sourceId == 0, "sound source id '%u' not released"
            , m_sourceId
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSource::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::finalize()
    {
        this->releaseSourceId_();

        m_soundBuffer = nullptr;
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
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        if( m_pausing == false )
        {
            OpenALSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            m_sourceId = soundSystemExtension->genSourceId();

            if( m_sourceId == 0 )
            {
                return false;
            }

            this->apply_( m_sourceId );

            if( m_soundBuffer->playSource( m_sourceId, m_loop, m_time ) == false )
            {
                LOGGER_ASSERTION( "invalid buffer play [%u] loop [%u] time [%f]"
                    , m_sourceId
                    , m_loop
                    , m_time
                );

                return false;
            }
        }
        else
        {
            m_soundBuffer->resumeSource( m_sourceId );
        }

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
            LOGGER_ASSERTION( "invalid sound buffer" );

            return;
        }

        if( m_sourceId == 0 )
        {
            return;
        }

        m_playing = true;
        m_pausing = true;

        MENGINE_OPENAL_CALL( alSourcef, (m_sourceId, AL_GAIN, 0.f) );

        m_soundBuffer->pauseSource( m_sourceId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSource::resume()
    {
        if( m_playing == false )
        {
            return false;
        }

        if( m_pausing == false )
        {
            return false;
        }

        if( m_soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        ALfloat gain = (ALfloat)StdMath::powf( m_volume, 2.f );
        MENGINE_OPENAL_CALL( alSourcef, (m_sourceId, AL_GAIN, gain) );

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

        m_time = 0.f;
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
        if( mt::equal_f_f( m_volume, _volume ) == true )
        {
            return;
        }

        m_volume = _volume;

        if( m_playing == true && m_sourceId != 0 )
        {
            ALfloat gain = (ALfloat)StdMath::powf( m_volume, 2.f );
            MENGINE_OPENAL_CALL( alSourcef, (m_sourceId, AL_GAIN, gain) );
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
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        float time_sound = m_soundBuffer->getTimeTotal();

        return time_sound;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSource::setPosition( float _posMs )
    {
        if( StdMath::fabsf( m_time - _posMs ) < 0.01f )
        {
            return true;
        }

        m_time = _posMs;

        if( m_playing == false && m_pausing == false )
        {
            return true;
        }

        float posmc = _posMs;

        float total = m_soundBuffer->getTimeTotal();

        if( posmc > total )
        {
            LOGGER_ASSERTION( "pos %f total %f"
                , posmc
                , total
            );

            return false;
        }

        if( posmc < 0.f )
        {
            LOGGER_ASSERTION( "pos %f less zero"
                , posmc
            );

            posmc = 0.f;
        }

        if( m_soundBuffer->setTimePos( m_sourceId, posmc ) == false )
        {
            LOGGER_ASSERTION( "invalid set time pos [%u] time [%f] (play %u)"
                , m_sourceId
                , posmc
                , m_playing
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenALSoundSource::getPosition() const
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        if( m_sourceId == 0 )
        {
            return m_time;
        }

        if( m_pausing == true )
        {
            return m_time;
        }

        float posms = 0.f;
        if( m_soundBuffer->getTimePos( m_sourceId, &posms ) == false )
        {
            LOGGER_ASSERTION( "invalid get time pos [%u] (play %u)"
                , m_sourceId
                , m_playing
            );

            return 0.f;
        }

        //timing dont assign to zero when m_soundBuffer is stopped!
        if( StdMath::fabsf( posms ) < 0.0001f && StdMath::fabsf( m_time ) > 0.0001f )
        {
            posms = m_time;
        }

        return posms;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->unloadBuffer_();

        m_soundBuffer = OpenALSoundBufferBasePtr::from( _soundBuffer );
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

            OpenALSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            soundSystemExtension->releaseSourceId( sourceId );
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
            MENGINE_OPENAL_CALL( alSourcei, (_source, AL_SOURCE_RELATIVE, AL_TRUE) );
            MENGINE_OPENAL_CALL( alSourcef, (_source, AL_ROLLOFF_FACTOR, 0.f) );
        }
        else
        {
            MENGINE_OPENAL_CALL( alSourcei, (_source, AL_SOURCE_RELATIVE, AL_FALSE) );
            MENGINE_OPENAL_CALL( alSourcef, (_source, AL_ROLLOFF_FACTOR, 1.f) );
        }

        MENGINE_OPENAL_CALL( alSourcei, (_source, AL_LOOPING, m_loop == true ? AL_TRUE : AL_FALSE) );
        MENGINE_OPENAL_CALL( alSource3f, (_source, AL_POSITION, 0.f, 0.f, -1.f) );
        MENGINE_OPENAL_CALL( alSource3f, (_source, AL_VELOCITY, 0.f, 0.f, 0.f) );
        MENGINE_OPENAL_CALL( alSourcef, (_source, AL_MIN_GAIN, 0.f) );
        MENGINE_OPENAL_CALL( alSourcef, (_source, AL_MAX_GAIN, 1.f) );
        MENGINE_OPENAL_CALL( alSourcef, (_source, AL_PITCH, 1.f) );

        ALfloat gain = (ALfloat)StdMath::powf( m_volume, 2.f );
        MENGINE_OPENAL_CALL( alSourcef, (_source, AL_GAIN, gain) );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & OpenALSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
}
