#include "AndroidSoundSource.h"

#include "AndroidSoundBufferBase.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#include "Config/StdMath.h"

#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static float calcGain( float _volume )
        {
            float gain = StdMath::powf( _volume, 2.f );

            return gain;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundSource::AndroidSoundSource()
        : m_volume( 1.f )
        , m_time( 0.f )
        , m_headMode( true )
        , m_playing( false )
        , m_pausing( false )
        , m_loop( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundSource::~AndroidSoundSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::finalize()
    {
        if( m_soundBuffer != nullptr )
        {
            if( m_playing == true || m_pausing == true )
            {
                m_soundBuffer->stopSource();
            }
        }

        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::play()
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
            float gain = Detail::calcGain( m_volume );

            m_soundBuffer->setVolume( gain );

            if( m_soundBuffer->playSource( m_loop, m_time ) == false )
            {
                LOGGER_ASSERTION( "invalid buffer play loop [%u] time [%f]"
                    , m_loop
                    , m_time
                );

                return false;
            }
        }
        else
        {
            float gain = Detail::calcGain( m_volume );

            m_soundBuffer->setVolume( gain );
            m_soundBuffer->resumeSource();
        }

        m_playing = true;
        m_pausing = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::pause()
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

        float position = 0.f;

        if( m_soundBuffer->getTimePosition( &position ) == true )
        {
            m_time = position;
        }

        m_playing = true;
        m_pausing = true;

        m_soundBuffer->setVolume( 0.f );
        m_soundBuffer->pauseSource();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::resume()
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

        float gain = Detail::calcGain( m_volume );

        m_soundBuffer->setVolume( gain );
        m_soundBuffer->resumeSource();

        m_playing = true;
        m_pausing = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::stop()
    {
        if( m_playing == false && m_pausing == false )
        {
            return;
        }

        m_playing = false;
        m_pausing = false;

        if( m_soundBuffer != nullptr )
        {
            m_soundBuffer->stopSource();
        }

        m_time = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::isPlay() const
    {
        return m_playing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::isPause() const
    {
        return m_pausing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::isFinished() const
    {
        if( m_soundBuffer == nullptr )
        {
            return false;
        }

        return m_soundBuffer->isFinished();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::setVolume( float _volume )
    {
        if( mt::equal_f_f( m_volume, _volume ) == true )
        {
            return;
        }

        m_volume = _volume;

        if( m_playing == true && m_soundBuffer != nullptr )
        {
            float gain = Detail::calcGain( m_volume );

            m_soundBuffer->setVolume( gain );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidSoundSource::getVolume() const
    {
        return m_volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::getLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidSoundSource::getDuration() const
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        float time_sound = m_soundBuffer->getTimeDuration();

        return time_sound;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::setPosition( float _position )
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        float position = _position;
        float total = m_soundBuffer->getTimeDuration();

        if( position > total )
        {
            LOGGER_ASSERTION( "pos %f total %f"
                , position
                , total
            );

            return false;
        }

        if( position < 0.f )
        {
            LOGGER_ASSERTION( "pos %f less zero"
                , position
            );

            position = 0.f;
        }

        if( m_playing == false && m_pausing == false )
        {
            if( mt::equal_f_f( m_time, position ) == true )
            {
                return true;
            }

            m_time = position;

            return true;
        }

        float currentPosition = this->getPosition();

        if( mt::equal_f_f( currentPosition, position ) == true )
        {
            m_time = position;

            return true;
        }

        if( m_soundBuffer->setTimePosition( position ) == false )
        {
            LOGGER_ASSERTION( "invalid set time position %f (play %u)"
                , position
                , m_playing
            );

            return false;
        }

        m_time = position;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidSoundSource::getPosition() const
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        if( m_pausing == true )
        {
            return m_time;
        }

        float position = 0.f;
        if( m_soundBuffer->getTimePosition( &position ) == false )
        {
            LOGGER_ASSERTION( "invalid get time position (play %u)"
                , m_playing
            );

            return 0.f;
        }

        if( mt::equal_f_z( position ) == true && mt::equal_f_z( m_time ) == false )
        {
            position = m_time;
        }

        return position;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->unloadBuffer_();

        m_soundBuffer = AndroidSoundBufferBasePtr::from( _soundBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & AndroidSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::unloadBuffer_()
    {
        if( m_soundBuffer != nullptr && m_playing == true )
        {
            this->stop();
        }

        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::setHeadMode( bool _headMode )
    {
        m_headMode = _headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::getHeadMode() const
    {
        return m_headMode;
    }
    //////////////////////////////////////////////////////////////////////////
}
