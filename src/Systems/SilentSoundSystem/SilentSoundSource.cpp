#include "SilentSoundSource.h"

#include "SilentSoundSystem.h"
#include "SilentSoundBuffer.h"

#include "Kernel/Logger.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SilentSoundSource::SilentSoundSource()
        : m_soundSystem( nullptr )
        , m_headMode( true )
        , m_playing( false )
        , m_pausing( false )
        , m_volume( 1.f )
        , m_loop( false )
        , m_sourceId( 0 )
        , m_time( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SilentSoundSource::~SilentSoundSource()
    {
        this->stop();
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSource::initialize( SilentSoundSystem * _soundSystem )
    {
        m_soundSystem = _soundSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSource::play()
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
            m_sourceId = m_soundSystem->genSourceId();

            if( m_sourceId == INVALID_UNIQUE_ID )
            {
                return false;
            }

            m_soundBuffer->play( m_sourceId, m_loop, m_time );
        }
        else
        {
            m_soundBuffer->resume( m_sourceId );
        }

        m_playing = true;
        m_pausing = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSource::pause()
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

        if( m_sourceId == INVALID_UNIQUE_ID )
        {
            return;
        }

        m_playing = false;
        m_pausing = true;

        m_time = m_soundBuffer->getTimePos( m_sourceId );

        m_soundBuffer->pause( m_sourceId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSource::resume()
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
            return false;
        }

        m_soundBuffer->resume( m_sourceId );

        m_playing = true;
        m_pausing = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSource::stop()
    {
        if( m_playing == false && m_pausing == false )
        {
            return;
        }

        m_playing = false;
        m_pausing = false;

        if( m_sourceId != 0 )
        {
            uint32_t sourceId = m_sourceId;
            m_sourceId = 0;

            m_soundBuffer->stop( sourceId );
            m_soundSystem->releaseSourceId( sourceId );
        }

        m_time = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSource::isPlay() const
    {
        return m_playing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSource::isPause() const
    {
        return m_pausing;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSource::setVolume( float _volume )
    {
        m_volume = _volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SilentSoundSource::getVolume() const
    {
        return m_volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSource::getLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    float SilentSoundSource::getDuration() const
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        float ms = m_soundBuffer->getTimeTotal();

        return ms;
    }
    //////////////////////////////////////////////////////////////////////////
    float SilentSoundSource::getPosition() const
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        if( m_sourceId == 0 )
        {
            return 0.f;
        }

        if( m_pausing == true )
        {
            return m_time;
        }

        float posms = m_soundBuffer->getTimePos( m_sourceId );

        //timing dont assign to zero when m_soundBuffer is stopped!
        if( MENGINE_FABSF( posms ) < 0.0001f && MENGINE_FABSF( m_time ) > 0.0001f )
        {
            posms = m_time;
        }

        return posms;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSource::setPosition( float _posMs )
    {
        if( m_playing == true )
        {
            return false;
        }

        m_time = _posMs;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->unloadBuffer_();

        m_soundBuffer = SilentSoundBufferPtr::from( _soundBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSource::unloadBuffer_()
    {
        if( m_soundBuffer != nullptr && m_playing == true )
        {
            this->stop();
        }

        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void SilentSoundSource::setHeadMode( bool _headMode )
    {
        m_headMode = _headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SilentSoundSource::getHeadMode() const
    {
        return m_headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & SilentSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
}
