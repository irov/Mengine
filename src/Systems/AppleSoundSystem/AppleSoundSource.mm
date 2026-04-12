#include "AppleSoundSource.h"

#include "AppleSoundSystemExtensionInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/SpinLockScope.h"

#include "Config/StdMath.h"
#include "Config/StdThread.h"

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
    AppleSoundSource::AppleSoundSource()
        : m_volume( 1.f )
        , m_framePosition( 0 )
        , m_headMode( true )
        , m_playing( false )
        , m_pausing( false )
        , m_loop( false )
        , m_finished( false )
        , m_renderBarrier( false )
        , m_activeRenders( 0 )
        , m_currentGain( 0.f )
        , m_busIndex( INVALID_BUS_INDEX )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSoundSource::~AppleSoundSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSource::finalize()
    {
        this->stop();

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::play()
    {
        if( m_playing.load() == true )
        {
            return true;
        }

        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        float initialGain = Detail::calcGain( m_volume.load() );
        m_currentGain = initialGain;

        LOGGER_MESSAGE_RELEASE( "[DIAG] AppleSoundSource::play() gain=%.3f pausing=%u loop=%u pos=%.1f"
            , initialGain
            , m_pausing.load()
            , m_loop.load()
            , this->getPosition()
        );

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

            m_playing = true;
            m_pausing = false;
            m_finished = false;

            if( this->acquireSourceBus_( soundBuffer->getFrequency(), soundBuffer->getChannels(), 1.f ) == false )
            {
                soundBuffer->stopSource();
                m_playing = false;
                m_pausing = false;
                m_finished = false;

                LOGGER_ASSERTION( "invalid acquire source bus" );

                return false;
            }

            LOGGER_MESSAGE_RELEASE( "[DIAG] AppleSoundSource::play() bus acquired, busIndex=%u freq=%u ch=%u"
                , m_busIndex
                , soundBuffer->getFrequency()
                , soundBuffer->getChannels()
            );
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
                if( this->acquireSourceBus_( soundBuffer->getFrequency(), soundBuffer->getChannels(), 1.f ) == false )
                {
                    LOGGER_ASSERTION( "invalid reacquire source bus" );

                    return false;
                }

                MENGINE_SPINLOCK_SCOPE( m_lock );
                busIndex = m_busIndex;
            }

            AppleSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            if( soundSystemExtension->setSourceBusVolume( busIndex, 1.f ) == false )
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
    void AppleSoundSource::pause()
    {
        if( m_playing.load() == false )
        {
            return;
        }

        if( m_pausing.load() == true )
        {
            return;
        }

        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

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
            AppleSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            soundSystemExtension->enableSourceBus( busIndex, false );
        }

        soundBuffer->pauseSource();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::resume()
    {
        if( m_playing.load() == false )
        {
            return false;
        }

        if( m_pausing.load() == false )
        {
            return false;
        }

        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        m_currentGain = Detail::calcGain( m_volume.load() );

        uint32_t busIndex;

        {
            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        if( busIndex == INVALID_BUS_INDEX )
        {
            if( this->acquireSourceBus_( soundBuffer->getFrequency(), soundBuffer->getChannels(), 1.f ) == false )
            {
                LOGGER_ASSERTION( "invalid reacquire source bus" );

                return false;
            }

            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        AppleSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        if( soundSystemExtension->setSourceBusVolume( busIndex, 1.f ) == false )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSource::stop()
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

        LOGGER_MESSAGE_RELEASE( "[DIAG] AppleSoundSource::stop() BEGIN playing=%u pausing=%u busIndex=%u pos=%.1f"
            , m_playing.load()
            , m_pausing.load()
            , busIndex
            , this->getPosition()
        );

        this->beginMutableState_();

        m_playing = false;
        m_pausing = false;
        m_finished = false;
        m_currentGain = 0.f;

        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer != nullptr )
        {
            soundBuffer->stopSource();
        }

        this->releaseSourceBus_();

        m_framePosition = 0;

        this->endMutableState_();

        LOGGER_MESSAGE_RELEASE( "[DIAG] AppleSoundSource::stop() END busIndex was %u"
            , busIndex
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::isPlay() const
    {
        return m_playing.load();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::isPause() const
    {
        return m_pausing.load();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::isFinished() const
    {
        return m_finished.load();
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSource::setVolume( float _volume )
    {
        m_volume = _volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float AppleSoundSource::getVolume() const
    {
        return m_volume.load();
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::getLoop() const
    {
        return m_loop.load();
    }
    //////////////////////////////////////////////////////////////////////////
    float AppleSoundSource::getDuration() const
    {
        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        float time_sound = soundBuffer->getTimeDuration();

        return time_sound;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::setPosition( float _position )
    {
        float currentPosition = this->getPosition();

        if( mt::equal_f_f( currentPosition, _position ) == true )
        {
            return true;
        }

        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        float position = _position;

        if( position < 0.f )
        {
            position = 0.f;
        }

        uint32_t totalFrames = soundBuffer->getFrameCount();

        if( totalFrames != 0 )
        {
            if( m_playing.load() == true || m_pausing.load() == true )
            {
                float total = soundBuffer->getTimeDuration();

                if( position > total )
                {
                    LOGGER_ASSERTION( "pos %f total %f"
                        , position
                        , total
                    );

                    return false;
                }
            }

            uint32_t frequency = soundBuffer->getFrequency();
            uint32_t positionFrame = (uint32_t)(position * (float)frequency / 1000.f);

            if( positionFrame > totalFrames )
            {
                positionFrame = totalFrames;
            }

            m_framePosition = positionFrame;

            return true;
        }

        if( m_playing.load() == true || m_pausing.load() == true )
        {
            float total = soundBuffer->getTimeDuration();

            if( position > total )
            {
                LOGGER_ASSERTION( "pos %f total %f"
                    , position
                    , total
                );

                return false;
            }
        }

        if( soundBuffer->setTimePosition( position ) == false )
        {
            LOGGER_ASSERTION( "invalid set time position %f (play %u)"
                , position
                , m_playing.load()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float AppleSoundSource::getPosition() const
    {
        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

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
    void AppleSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->unloadBuffer_();

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = AppleSoundBufferBasePtr::from( _soundBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & AppleSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSource::unloadBuffer_()
    {
        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();
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
    void AppleSoundSource::setHeadMode( bool _headMode )
    {
        m_headMode = _headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::getHeadMode() const
    {
        return m_headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    OSStatus AppleSoundSource::renderMixerFrames( UInt32 _frames, AudioBufferList * _ioData )
    {
        if( _ioData == nullptr )
        {
            return noErr;
        }

        if( this->tryEnterRender_() == false )
        {
            LOGGER_MESSAGE_RELEASE( "[DIAG] renderMixerFrames BLOCKED by barrier frames=%u"
                , (uint32_t)_frames
            );

            return noErr;
        }

        if( m_playing.load() == false || m_pausing.load() == true )
        {
            this->leaveRender_();

            return noErr;
        }

        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            this->leaveRender_();

            return noErr;
        }

        uint32_t currentFrame = m_framePosition.load();
        uint32_t newFrame = currentFrame;

        uint32_t renderedFrames = soundBuffer->renderMixerFrames( _ioData, 0, (uint32_t)_frames, currentFrame, m_loop.load(), &newFrame );

        m_framePosition = newFrame;

        // Per-sample gain ramp (volume applied here instead of AudioUnitSetParameter)
        {
            float targetGain = Detail::calcGain( m_volume.load() );
            float startGain = m_currentGain.load();

            if( renderedFrames != 0 )
            {
                float gainInc = (renderedFrames > 1) ? (targetGain - startGain) / (float)renderedFrames : 0.f;

                for( UInt32 bufIdx = 0; bufIdx != _ioData->mNumberBuffers; ++bufIdx )
                {
                    Float32 * dst = static_cast<Float32 *>(_ioData->mBuffers[bufIdx].mData);

                    if( dst == nullptr )
                    {
                        continue;
                    }

                    UInt32 numChannels = _ioData->mBuffers[bufIdx].mNumberChannels;
                    float g = startGain;

                    for( uint32_t frame = 0; frame != renderedFrames; ++frame )
                    {
                        for( UInt32 ch = 0; ch != numChannels; ++ch )
                        {
                            dst[frame * numChannels + ch] *= g;
                        }

                        g += gainInc;
                    }
                }

                m_currentGain = targetGain;
            }
        }

        if( renderedFrames != 0 && renderedFrames < (uint32_t)_frames )
        {
            LOGGER_MESSAGE_RELEASE( "[DIAG] renderMixerFrames PARTIAL rendered=%u/%u frame=%u"
                , renderedFrames
                , (uint32_t)_frames
                , newFrame
            );
        }

        uint32_t totalFrames = soundBuffer->getFrameCount();

        if( totalFrames != 0 && renderedFrames < (uint32_t)_frames && m_loop.load() == false )
        {
            LOGGER_MESSAGE_RELEASE( "[DIAG] renderMixerFrames FINISHED rendered=%u/%u totalFrames=%u"
                , renderedFrames
                , (uint32_t)_frames
                , totalFrames
            );

            m_finished = true;
        }

        this->leaveRender_();

        return noErr;
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferBasePtr AppleSoundSource::getSoundBuffer_() const
    {
        MENGINE_SPINLOCK_SCOPE( m_lock );

        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::acquireSourceBus_( uint32_t _frequency, uint32_t _channels, float _gain )
    {
        if( _frequency == 0 || _channels == 0 )
        {
            return false;
        }

        AppleSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        uint32_t busIndex = INVALID_BUS_INDEX;

        if( soundSystemExtension->acquireSourceBus( AppleSoundSourcePtr::from( this ), _frequency, _channels, _gain, &busIndex ) == false )
        {
            return false;
        }

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_busIndex = busIndex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSource::releaseSourceBus_()
    {
        uint32_t busIndex = this->detachBusIndex_();

        if( busIndex == INVALID_BUS_INDEX )
        {
            return;
        }

        AppleSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        soundSystemExtension->releaseSourceBus( busIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundSource::detachBusIndex_()
    {
        MENGINE_SPINLOCK_SCOPE( m_lock );

        uint32_t busIndex = m_busIndex;
        m_busIndex = INVALID_BUS_INDEX;

        return busIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSource::beginMutableState_()
    {
        m_renderBarrier = true;

        while( m_activeRenders.load() != 0 )
        {
            StdThread::yield();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSource::endMutableState_()
    {
        m_renderBarrier = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSource::tryEnterRender_()
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
    void AppleSoundSource::leaveRender_()
    {
        m_activeRenders.fetch_sub( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
}
