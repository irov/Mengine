#include "WASAPISoundSource.h"

#include "WASAPISoundSystemExtensionInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/SpinLockScope.h"

#include "Config/StdMath.h"
#include "Config/StdThread.h"

#include "math/utils.h"

namespace Mengine
{
    namespace Detail
    {
        static float calcGain( float _volume )
        {
            return StdMath::powf( _volume, 2.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundSource::WASAPISoundSource()
        : m_volume( 1.f )
        , m_framePosition( 0 )
        , m_headMode( true )
        , m_playing( false )
        , m_pausing( false )
        , m_loop( false )
        , m_finished( false )
        , m_renderBarrier( false )
        , m_activeRenders( 0 )
        , m_busIndex( INVALID_BUS_INDEX )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundSource::~WASAPISoundSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::finalize()
    {
        this->stop();

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::play()
    {
        if( m_playing.load() == true )
        {
            return true;
        }

        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        float gain = Detail::calcGain( m_volume.load() );

        if( m_pausing.load() == false )
        {
            float position = this->getPosition();

            if( soundBuffer->playSource( m_loop.load(), position ) == false )
            {
                LOGGER_ASSERTION( "invalid buffer play loop [%u] time [%f]"
                    , m_loop.load()
                    , position
                );

                return false;
            }

            if( this->acquireSourceBus_( soundBuffer->getFrequency(), soundBuffer->getChannels(), gain ) == false )
            {
                soundBuffer->stopSource();

                LOGGER_ASSERTION( "invalid acquire source bus" );

                return false;
            }
        }
        else
        {
            uint32_t busIndex;

            {
                MENGINE_SPINLOCK_SCOPE( m_lock );
                busIndex = m_busIndex;
            }

            if( busIndex == INVALID_BUS_INDEX )
            {
                if( this->acquireSourceBus_( soundBuffer->getFrequency(), soundBuffer->getChannels(), gain ) == false )
                {
                    LOGGER_ASSERTION( "invalid reacquire source bus" );

                    return false;
                }

                MENGINE_SPINLOCK_SCOPE( m_lock );
                busIndex = m_busIndex;
            }

            WASAPISoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            if( soundSystemExtension->setSourceBusVolume( busIndex, gain ) == false )
            {
                LOGGER_ASSERTION( "invalid restore source bus volume [%u]"
                    , busIndex
                );

                return false;
            }

            soundSystemExtension->enableSourceBus( busIndex, true );

            soundBuffer->resumeSource();
        }

        m_playing = true;
        m_pausing = false;
        m_finished = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::pause()
    {
        if( m_playing.load() == false || m_pausing.load() == true )
        {
            return;
        }

        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return;
        }

        m_playing = true;
        m_pausing = true;

        uint32_t busIndex;

        {
            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        if( busIndex != INVALID_BUS_INDEX )
        {
            WASAPISoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            soundSystemExtension->enableSourceBus( busIndex, false );
        }

        soundBuffer->pauseSource();
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::resume()
    {
        if( m_playing.load() == false || m_pausing.load() == false )
        {
            return false;
        }

        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        float gain = Detail::calcGain( m_volume.load() );
        uint32_t busIndex;

        {
            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        if( busIndex == INVALID_BUS_INDEX )
        {
            if( this->acquireSourceBus_( soundBuffer->getFrequency(), soundBuffer->getChannels(), gain ) == false )
            {
                LOGGER_ASSERTION( "invalid reacquire source bus" );

                return false;
            }

            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        WASAPISoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        if( soundSystemExtension->setSourceBusVolume( busIndex, gain ) == false )
        {
            LOGGER_ASSERTION( "invalid restore source bus volume [%u]"
                , busIndex
            );

            return false;
        }

        soundSystemExtension->enableSourceBus( busIndex, true );

        soundBuffer->resumeSource();

        m_playing = true;
        m_pausing = false;
        m_finished = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::stop()
    {
        uint32_t busIndex;

        {
            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        if( m_playing.load() == false && m_pausing.load() == false && busIndex == INVALID_BUS_INDEX )
        {
            return;
        }

        this->beginMutableState_();

        m_playing = false;
        m_pausing = false;
        m_finished = false;

        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer != nullptr )
        {
            soundBuffer->stopSource();
        }

        this->releaseSourceBus_();

        m_framePosition = 0;

        this->endMutableState_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::isPlay() const
    {
        return m_playing.load();
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::isPause() const
    {
        return m_pausing.load();
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::isFinished() const
    {
        return m_finished.load();
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::setVolume( float _volume )
    {
        if( mt::equal_f_f( m_volume.load(), _volume ) == true )
        {
            return;
        }

        m_volume = _volume;

        if( m_playing.load() == true && m_pausing.load() == false )
        {
            float gain = Detail::calcGain( m_volume.load() );
            uint32_t busIndex;

            {
                MENGINE_SPINLOCK_SCOPE( m_lock );
                busIndex = m_busIndex;
            }

            if( busIndex != INVALID_BUS_INDEX )
            {
                WASAPISoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                    ->getUnknown();

                soundSystemExtension->setSourceBusVolume( busIndex, gain );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float WASAPISoundSource::getVolume() const
    {
        return m_volume.load();
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::setLoop( bool _loop )
    {
        bool previousLoop = m_loop.exchange( _loop );
        bool previousFinished = m_finished.load();

        if( _loop == true )
        {
            m_finished = false;
        }

        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer != nullptr )
        {
            if( soundBuffer->setLoopSource( _loop ) == false )
            {
                m_loop = previousLoop;
                m_finished = previousFinished;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::getLoop() const
    {
        return m_loop.load();
    }
    //////////////////////////////////////////////////////////////////////////
    float WASAPISoundSource::getDuration() const
    {
        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        return soundBuffer->getTimeDuration();
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::setPosition( float _position )
    {
        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        this->beginMutableState_();

        float currentPosition = this->getPosition();

        if( mt::equal_f_f( currentPosition, _position ) == true )
        {
            this->endMutableState_();

            return true;
        }

        float position = _position;

        if( position < 0.f )
        {
            position = 0.f;
        }

        float total = soundBuffer->getTimeDuration();

        if( position > total )
        {
            LOGGER_ASSERTION( "pos %f total %f"
                , position
                , total
            );

            this->endMutableState_();

            return false;
        }

        uint32_t totalFrames = soundBuffer->getFrameCount();

        if( totalFrames != 0 )
        {
            uint32_t frequency = soundBuffer->getFrequency();
            uint32_t positionFrame;

            if( total > 0.f && position >= total )
            {
                positionFrame = totalFrames;
            }
            else
            {
                positionFrame = (uint32_t)(position * (float)frequency / 1000.f);

                if( positionFrame > totalFrames )
                {
                    positionFrame = totalFrames;
                }
            }

            m_framePosition = positionFrame;
            m_finished = m_loop.load() == false && total > 0.f && position >= total;

            this->endMutableState_();

            return true;
        }

        if( soundBuffer->setTimePosition( position ) == false )
        {
            LOGGER_ASSERTION( "invalid set time position %f (play %u)"
                , position
                , m_playing.load()
            );

            this->endMutableState_();

            return false;
        }

        m_finished = m_loop.load() == false && total > 0.f && position >= total;

        this->endMutableState_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float WASAPISoundSource::getPosition() const
    {
        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        uint32_t totalFrames = soundBuffer->getFrameCount();

        if( totalFrames != 0 )
        {
            uint32_t frequency = soundBuffer->getFrequency();

            if( frequency == 0 )
            {
                return 0.f;
            }

            float position = (float)m_framePosition.load() * 1000.f / (float)frequency;

            if( position > soundBuffer->getTimeDuration() )
            {
                position = soundBuffer->getTimeDuration();
            }

            return position;
        }

        float position = 0.f;

        if( soundBuffer->getTimePosition( &position ) == false )
        {
            LOGGER_ASSERTION( "invalid get time position (play %u)"
                , m_playing.load()
            );

            return 0.f;
        }

        return position;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->unloadBuffer_();

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = WASAPISoundBufferBasePtr::from( _soundBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & WASAPISoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::setHeadMode( bool _headMode )
    {
        m_headMode = _headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::getHeadMode() const
    {
        return m_headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::renderMixerFrames( uint32_t _frames, uint32_t _outputChannels, float * _mixBuffer, float _gain )
    {
        if( _mixBuffer == nullptr || _frames == 0 || _outputChannels == 0 )
        {
            return;
        }

        if( this->tryEnterRender_() == false )
        {
            return;
        }

        if( m_playing.load() == false || m_pausing.load() == true )
        {
            this->leaveRender_();

            return;
        }

        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            this->leaveRender_();

            return;
        }

        uint32_t currentFrame = m_framePosition.load();
        uint32_t newFrame = currentFrame;

        uint32_t renderedFrames = soundBuffer->renderMixerFrames( _mixBuffer, _outputChannels, 0, _frames, _gain, currentFrame, m_loop.load(), &newFrame );

        m_framePosition = newFrame;

        uint32_t totalFrames = soundBuffer->getFrameCount();

        if( renderedFrames < _frames && m_loop.load() == false && (totalFrames != 0 || soundBuffer->isEndOfStream() == true) )
        {
            m_finished = true;
        }

        this->leaveRender_();
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::unloadBuffer_()
    {
        WASAPISoundBufferBasePtr soundBuffer = this->getSoundBuffer_();
        uint32_t busIndex;

        {
            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        if( soundBuffer != nullptr && (m_playing.load() == true || m_pausing.load() == true || busIndex != INVALID_BUS_INDEX) )
        {
            this->stop();
        }

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundBufferBasePtr WASAPISoundSource::getSoundBuffer_() const
    {
        MENGINE_SPINLOCK_SCOPE( m_lock );

        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::acquireSourceBus_( uint32_t _frequency, uint32_t _channels, float _gain )
    {
        if( _frequency == 0 || _channels == 0 )
        {
            return false;
        }

        WASAPISoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        uint32_t busIndex = INVALID_BUS_INDEX;

        if( soundSystemExtension->acquireSourceBus( WASAPISoundSourcePtr::from( this ), _frequency, _channels, _gain, &busIndex ) == false )
        {
            return false;
        }

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_busIndex = busIndex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::releaseSourceBus_()
    {
        uint32_t busIndex = this->detachBusIndex_();

        if( busIndex == INVALID_BUS_INDEX )
        {
            return;
        }

        WASAPISoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        soundSystemExtension->releaseSourceBus( busIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundSource::detachBusIndex_()
    {
        MENGINE_SPINLOCK_SCOPE( m_lock );

        uint32_t busIndex = m_busIndex;
        m_busIndex = INVALID_BUS_INDEX;

        return busIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::beginMutableState_()
    {
        m_renderBarrier = true;

        while( m_activeRenders.load() != 0 )
        {
            StdThread::yield();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::endMutableState_()
    {
        m_renderBarrier = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSource::tryEnterRender_()
    {
        for( ;; )
        {
            if( m_renderBarrier.load() == true )
            {
                return false;
            }

            m_activeRenders.fetch_add( 1 );

            if( m_renderBarrier.load() == false )
            {
                return true;
            }

            m_activeRenders.fetch_sub( 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSource::leaveRender_()
    {
        m_activeRenders.fetch_sub( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
}
