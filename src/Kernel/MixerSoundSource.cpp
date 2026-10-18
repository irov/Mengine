#include "MixerSoundSource.h"

#include "Kernel/MixerSoundExtensionInterface.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/SpinLockScope.h"
#include "Kernel/SpinLockTryScope.h"

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
    MixerSoundSource::MixerSoundSource()
        : m_volume( 1.f )
        , m_framePosition( 0 )
        , m_headMode( true )
        , m_playing( false )
        , m_pausing( false )
        , m_loop( false )
        , m_finished( false )
        , m_busIndex( INVALID_BUS_INDEX )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MixerSoundSource::~MixerSoundSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundSource::finalize()
    {
        this->stop();

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::play()
    {
        if( m_pausing.load() == true )
        {
            if( this->resume() == false )
            {
                return false;
            }

            return true;
        }

        if( m_playing.load() == true )
        {
            return true;
        }

        MixerSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        float position = this->getPosition();
        bool loop = m_loop.load();

        if( soundBuffer->playSource( loop, position ) == false )
        {
            LOGGER_ASSERTION( "invalid buffer play loop [%u] time [%f]"
                , loop
                , position
            );

            return false;
        }

        float volume = m_volume.load();
        float gain = Detail::calcGain( volume );
        uint32_t frequency = soundBuffer->getFrequency();
        uint32_t channels = soundBuffer->getChannels();

        if( this->acquireSourceBus_( frequency, channels, gain ) == false )
        {
            soundBuffer->stopSource();

            LOGGER_ASSERTION( "invalid acquire source bus" );

            return false;
        }

        m_playing = true;
        m_pausing = false;
        m_finished = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundSource::pause()
    {
        if( m_playing.load() == false )
        {
            return;
        }

        if( m_pausing.load() == true )
        {
            return;
        }

        MixerSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return;
        }

        m_playing = true;
        m_pausing = true;

        uint32_t busIndex = this->getBusIndex_();

        if( busIndex != INVALID_BUS_INDEX )
        {
            MixerSoundExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            soundSystemExtension->enableSourceBus( busIndex, false );
        }

        soundBuffer->pauseSource();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::resume()
    {
        if( m_playing.load() == false )
        {
            return false;
        }

        if( m_pausing.load() == false )
        {
            return false;
        }

        MixerSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        float volume = m_volume.load();
        float gain = Detail::calcGain( volume );
        uint32_t busIndex = this->getBusIndex_();

        if( busIndex == INVALID_BUS_INDEX )
        {
            uint32_t frequency = soundBuffer->getFrequency();
            uint32_t channels = soundBuffer->getChannels();

            if( this->acquireSourceBus_( frequency, channels, gain ) == false )
            {
                LOGGER_ASSERTION( "invalid reacquire source bus" );

                return false;
            }

            busIndex = this->getBusIndex_();
        }

        MixerSoundExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
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
    void MixerSoundSource::stop()
    {
        uint32_t busIndex = this->getBusIndex_();

        if( m_playing.load() == false )
        {
            if( m_pausing.load() == false )
            {
                if( busIndex == INVALID_BUS_INDEX )
                {
                    return;
                }
            }
        }

        MENGINE_SPINLOCK_SCOPE( m_renderLock );

        m_playing = false;
        m_pausing = false;
        m_finished = false;

        MixerSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer != nullptr )
        {
            soundBuffer->stopSource();
        }

        this->releaseSourceBus_();

        m_framePosition = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::isPlay() const
    {
        if( m_playing.load() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::isPause() const
    {
        if( m_pausing.load() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::isFinished() const
    {
        if( m_finished.load() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundSource::setVolume( float _volume )
    {
        float volume = m_volume.load();

        if( mt::equal_f_f( volume, _volume ) == true )
        {
            return;
        }

        m_volume = _volume;

        if( m_playing.load() == false )
        {
            return;
        }

        if( m_pausing.load() == true )
        {
            return;
        }

        float gain = Detail::calcGain( _volume );
        uint32_t busIndex = this->getBusIndex_();

        if( busIndex == INVALID_BUS_INDEX )
        {
            return;
        }

        MixerSoundExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        soundSystemExtension->setSourceBusVolume( busIndex, gain );
    }
    //////////////////////////////////////////////////////////////////////////
    float MixerSoundSource::getVolume() const
    {
        float volume = m_volume.load();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundSource::setLoop( bool _loop )
    {
        bool previousLoop = m_loop.exchange( _loop );
        bool previousFinished = m_finished.load();

        if( _loop == true )
        {
            m_finished = false;
        }

        MixerSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            return;
        }

        if( soundBuffer->setLoopSource( _loop ) == false )
        {
            m_loop = previousLoop;
            m_finished = previousFinished;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::getLoop() const
    {
        if( m_loop.load() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float MixerSoundSource::getDuration() const
    {
        MixerSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        float duration = soundBuffer->getTimeDuration();

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::setPosition( float _position )
    {
        MENGINE_ASSERTION( StdMath::isfinite( _position ) == true, "invalid sound position %f"
            , _position
        );

        MixerSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        MENGINE_SPINLOCK_SCOPE( m_renderLock );

        float currentPosition = this->getPosition();

        if( mt::equal_f_f( currentPosition, _position ) == true )
        {
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

            return false;
        }

        uint32_t totalFrames = soundBuffer->getFrameCount();

        if( totalFrames != 0 )
        {
            uint32_t frequency = soundBuffer->getFrequency();

            if( total > 0.f )
            {
                if( position >= total )
                {
                    m_framePosition = totalFrames;
                    m_finished = m_loop.load() == false;

                    return true;
                }
            }

            uint32_t positionFrame = (uint32_t)(position * (float)frequency / 1000.f);

            if( positionFrame > totalFrames )
            {
                positionFrame = totalFrames;
            }

            m_framePosition = positionFrame;
            m_finished = false;

            return true;
        }

        if( soundBuffer->setTimePosition( position ) == false )
        {
            LOGGER_ASSERTION( "invalid set time position %f (play %u)"
                , position
                , m_playing.load()
            );

            return false;
        }

        m_finished = m_loop.load() == false && total > 0.f && position >= total;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float MixerSoundSource::getPosition() const
    {
        MixerSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

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

            float duration = soundBuffer->getTimeDuration();

            if( position > duration )
            {
                position = duration;
            }

            return position;
        }

        float position;
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
    void MixerSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->unloadBuffer_();

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = MixerSoundBufferBasePtr::from( _soundBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & MixerSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundSource::setHeadMode( bool _headMode )
    {
        m_headMode = _headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::getHeadMode() const
    {
        return m_headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundSource::renderMixerFrames( uint32_t _frames, uint32_t _outputChannels, float * _mixBuffer, float _gain )
    {
        if( _mixBuffer == nullptr )
        {
            return;
        }

        if( _frames == 0 )
        {
            return;
        }

        if( _outputChannels == 0 )
        {
            return;
        }

        SpinLockTryScope renderScope( m_renderLock );

        if( renderScope.isLocked() == false )
        {
            return;
        }

        if( m_playing.load() == false )
        {
            return;
        }

        if( m_pausing.load() == true )
        {
            return;
        }

        MixerSoundBufferBase * soundBuffer = m_soundBuffer.get();

        if( soundBuffer == nullptr )
        {
            return;
        }

        uint32_t currentFrame = m_framePosition.load();
        bool loop = m_loop.load();
        uint32_t newFrame;
        uint32_t renderedFrames = soundBuffer->renderMixerFrames( _mixBuffer, _outputChannels, 0, _frames, _gain, currentFrame, loop, &newFrame );

        m_framePosition = newFrame;

        uint32_t totalFrames = soundBuffer->getFrameCount();

        if( renderedFrames >= _frames )
        {
            return;
        }

        if( m_loop.load() == true )
        {
            return;
        }

        if( totalFrames == 0 )
        {
            if( soundBuffer->isEndOfStream() == false )
            {
                return;
            }
        }

        m_finished = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundSource::unloadBuffer_()
    {
        MixerSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            return;
        }

        this->stop();

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    MixerSoundBufferBasePtr MixerSoundSource::getSoundBuffer_() const
    {
        MENGINE_SPINLOCK_SCOPE( m_lock );

        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundSource::getBusIndex_() const
    {
        MENGINE_SPINLOCK_SCOPE( m_lock );

        return m_busIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundSource::acquireSourceBus_( uint32_t _frequency, uint32_t _channels, float _gain )
    {
        if( _frequency == 0 )
        {
            return false;
        }

        if( _channels == 0 )
        {
            return false;
        }

        MixerSoundExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        MixerSoundSourcePtr source = MixerSoundSourcePtr::from( this );
        uint32_t busIndex;
        if( soundSystemExtension->acquireSourceBus( source, _frequency, _channels, _gain, &busIndex ) == false )
        {
            return false;
        }

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_busIndex = busIndex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundSource::releaseSourceBus_()
    {
        uint32_t busIndex = this->detachBusIndex_();

        if( busIndex == INVALID_BUS_INDEX )
        {
            return;
        }

        MixerSoundExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        soundSystemExtension->releaseSourceBus( busIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundSource::detachBusIndex_()
    {
        MENGINE_SPINLOCK_SCOPE( m_lock );

        uint32_t busIndex = m_busIndex;
        m_busIndex = INVALID_BUS_INDEX;

        return busIndex;
    }
    //////////////////////////////////////////////////////////////////////////
}
