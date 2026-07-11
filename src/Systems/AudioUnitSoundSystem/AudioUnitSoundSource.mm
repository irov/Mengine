#include "AudioUnitSoundSource.h"

#include "AudioUnitSoundSystemExtensionInterface.h"

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
    AudioUnitSoundSource::AudioUnitSoundSource()
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
    AudioUnitSoundSource::~AudioUnitSoundSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSource::finalize()
    {
        this->stop();

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::play()
    {
        if( m_playing.load() == true )
        {
            return true;
        }

        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        float initialGain = Detail::calcGain( m_volume.load() );

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

            if( this->acquireSourceBus_( soundBuffer->getFrequency(), soundBuffer->getChannels(), initialGain ) == false )
            {
                soundBuffer->stopSource();
                m_playing = false;
                m_pausing = false;
                m_finished = false;

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
                if( this->acquireSourceBus_( soundBuffer->getFrequency(), soundBuffer->getChannels(), initialGain ) == false )
                {
                    LOGGER_ASSERTION( "invalid reacquire source bus" );

                    return false;
                }

                MENGINE_SPINLOCK_SCOPE( m_lock );
                busIndex = m_busIndex;
            }

            AudioUnitSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            if( soundSystemExtension->setSourceBusVolume( busIndex, initialGain ) == false )
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
    void AudioUnitSoundSource::pause()
    {
        if( m_playing.load() == false )
        {
            return;
        }

        if( m_pausing.load() == true )
        {
            return;
        }

        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

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
            AudioUnitSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            soundSystemExtension->enableSourceBus( busIndex, false );
        }

        soundBuffer->pauseSource();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::resume()
    {
        if( m_playing.load() == false )
        {
            return false;
        }

        if( m_pausing.load() == false )
        {
            return false;
        }

        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return false;
        }

        float resumeGain = Detail::calcGain( m_volume.load() );

        uint32_t busIndex;

        {
            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        if( busIndex == INVALID_BUS_INDEX )
        {
            if( this->acquireSourceBus_( soundBuffer->getFrequency(), soundBuffer->getChannels(), resumeGain ) == false )
            {
                LOGGER_ASSERTION( "invalid reacquire source bus" );

                return false;
            }

            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        AudioUnitSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        if( soundSystemExtension->setSourceBusVolume( busIndex, resumeGain ) == false )
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
    void AudioUnitSoundSource::stop()
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

        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer != nullptr )
        {
            soundBuffer->stopSource();
        }

        this->releaseSourceBus_();

        m_framePosition = 0;

        this->endMutableState_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::isPlay() const
    {
        return m_playing.load();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::isPause() const
    {
        return m_pausing.load();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::isFinished() const
    {
        return m_finished.load();
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSource::setVolume( float _volume )
    {
        m_volume = _volume;

        uint32_t busIndex;

        {
            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        if( busIndex != INVALID_BUS_INDEX )
        {
            float gain = Detail::calcGain( _volume );

            AudioUnitSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                ->getUnknown();

            soundSystemExtension->setSourceBusVolume( busIndex, gain );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float AudioUnitSoundSource::getVolume() const
    {
        return m_volume.load();
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::getLoop() const
    {
        return m_loop.load();
    }
    //////////////////////////////////////////////////////////////////////////
    float AudioUnitSoundSource::getDuration() const
    {
        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer" );

            return 0.f;
        }

        float time_sound = soundBuffer->getTimeDuration();

        return time_sound;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::setPosition( float _position )
    {
        float currentPosition = this->getPosition();

        if( mt::equal_f_f( currentPosition, _position ) == true )
        {
            return true;
        }

        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

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
    float AudioUnitSoundSource::getPosition() const
    {
        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

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
    void AudioUnitSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->unloadBuffer_();

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_soundBuffer = AudioUnitSoundBufferBasePtr::from( _soundBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & AudioUnitSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSource::unloadBuffer_()
    {
        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();
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
    void AudioUnitSoundSource::setHeadMode( bool _headMode )
    {
        m_headMode = _headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::getHeadMode() const
    {
        return m_headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    OSStatus AudioUnitSoundSource::renderMixerFrames( UInt32 _frames, AudioBufferList * _ioData )
    {
        if( _ioData == nullptr )
        {
            return noErr;
        }

        if( this->tryEnterRender_() == false )
        {
            return noErr;
        }

        if( m_playing.load() == false || m_pausing.load() == true )
        {
            this->leaveRender_();

            return noErr;
        }

        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            this->leaveRender_();

            return noErr;
        }

        uint32_t currentFrame = m_framePosition.load();
        uint32_t newFrame = currentFrame;

        uint32_t renderedFrames = soundBuffer->renderMixerFrames( _ioData, 0, (uint32_t)_frames, currentFrame, m_loop.load(), &newFrame );

        m_framePosition = newFrame;

        uint32_t totalFrames = soundBuffer->getFrameCount();

        if( totalFrames != 0 && renderedFrames < (uint32_t)_frames && m_loop.load() == false )
        {
            m_finished = true;
        }

        this->leaveRender_();

        return noErr;
    }
    //////////////////////////////////////////////////////////////////////////
    AudioUnitSoundBufferBasePtr AudioUnitSoundSource::getSoundBuffer_() const
    {
        MENGINE_SPINLOCK_SCOPE( m_lock );

        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::acquireSourceBus_( uint32_t _frequency, uint32_t _channels, float _gain )
    {
        if( _frequency == 0 || _channels == 0 )
        {
            return false;
        }

        AudioUnitSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        uint32_t busIndex = INVALID_BUS_INDEX;

        if( soundSystemExtension->acquireSourceBus( AudioUnitSoundSourcePtr::from( this ), _frequency, _channels, _gain, &busIndex ) == false )
        {
            return false;
        }

        MENGINE_SPINLOCK_SCOPE( m_lock );
        m_busIndex = busIndex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSource::releaseSourceBus_()
    {
        uint32_t busIndex = this->detachBusIndex_();

        if( busIndex == INVALID_BUS_INDEX )
        {
            return;
        }

        AudioUnitSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        soundSystemExtension->releaseSourceBus( busIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AudioUnitSoundSource::detachBusIndex_()
    {
        MENGINE_SPINLOCK_SCOPE( m_lock );

        uint32_t busIndex = m_busIndex;
        m_busIndex = INVALID_BUS_INDEX;

        return busIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSource::beginMutableState_()
    {
        m_renderBarrier = true;

        while( m_activeRenders.load() != 0 )
        {
            StdThread::yield();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSource::endMutableState_()
    {
        m_renderBarrier = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::tryEnterRender_()
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
    void AudioUnitSoundSource::leaveRender_()
    {
        m_activeRenders.fetch_sub( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
}
