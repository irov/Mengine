#include "AudioUnitSoundSource.h"

#include "AudioUnitSoundSystemExtensionInterface.h"

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
    AudioUnitSoundSource::AudioUnitSoundSource()
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

        float volume = m_volume.load();
        float initialGain = Detail::calcGain( volume );

        if( m_pausing.load() == false )
        {
            float position = this->getPosition();

            bool loop = m_loop.load();
            if( soundBuffer->playSource( loop, position ) == false )
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

            uint32_t frequency = soundBuffer->getFrequency();
            uint32_t channels = soundBuffer->getChannels();

            if( this->acquireSourceBus_( frequency, channels, initialGain ) == false )
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
                uint32_t frequency = soundBuffer->getFrequency();
                uint32_t channels = soundBuffer->getChannels();

                if( this->acquireSourceBus_( frequency, channels, initialGain ) == false )
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

        float volume = m_volume.load();
        float resumeGain = Detail::calcGain( volume );

        uint32_t busIndex;

        {
            MENGINE_SPINLOCK_SCOPE( m_lock );
            busIndex = m_busIndex;
        }

        if( busIndex == INVALID_BUS_INDEX )
        {
            uint32_t frequency = soundBuffer->getFrequency();
            uint32_t channels = soundBuffer->getChannels();

            if( this->acquireSourceBus_( frequency, channels, resumeGain ) == false )
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

        AudioUnitSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer != nullptr )
        {
            soundBuffer->stopSource();
        }

        this->releaseSourceBus_();

        m_framePosition = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::isPlay() const
    {
        if( m_playing.load() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::isPause() const
    {
        if( m_pausing.load() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::isFinished() const
    {
        if( m_finished.load() == false )
        {
            return false;
        }

        return true;
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

        if( busIndex == INVALID_BUS_INDEX )
        {
            return;
        }

        float gain = Detail::calcGain( _volume );

        AudioUnitSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        soundSystemExtension->setSourceBusVolume( busIndex, gain );
    }
    //////////////////////////////////////////////////////////////////////////
    float AudioUnitSoundSource::getVolume() const
    {
        float volume = m_volume.load();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::getLoop() const
    {
        if( m_loop.load() == false )
        {
            return false;
        }

        return true;
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

        float duration = soundBuffer->getTimeDuration();

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSource::setPosition( float _position )
    {
        MENGINE_ASSERTION( StdMath::isfinite( _position ) == true, "invalid sound position %f"
            , _position
        );

        MENGINE_SPINLOCK_SCOPE( m_renderLock );

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

        bool active = m_playing.load();

        if( active == false )
        {
            active = m_pausing.load();
        }

        if( active == true )
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

        if( totalFrames != 0 )
        {
            uint32_t frequency = soundBuffer->getFrequency();
            uint32_t positionFrame = (uint32_t)(position * (float)frequency / 1000.f);

            if( positionFrame > totalFrames )
            {
                positionFrame = totalFrames;
            }

            m_framePosition = positionFrame;

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

            float duration = soundBuffer->getTimeDuration();

            if( position > duration )
            {
                position = duration;
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

        if( soundBuffer != nullptr )
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

        SpinLockTryScope renderScope( m_renderLock );

        if( renderScope.isLocked() == false )
        {
            return noErr;
        }

        if( m_playing.load() == false )
        {
            return noErr;
        }

        if( m_pausing.load() == true )
        {
            return noErr;
        }

        AudioUnitSoundBufferBase * soundBuffer = m_soundBuffer.get();

        if( soundBuffer == nullptr )
        {
            return noErr;
        }

        uint32_t currentFrame = m_framePosition.load();
        uint32_t frames = (uint32_t)_frames;
        bool loop = m_loop.load();
        uint32_t newFrame = currentFrame;
        uint32_t renderedFrames = soundBuffer->renderMixerFrames( _ioData, 0, frames, currentFrame, loop, &newFrame );

        m_framePosition = newFrame;

        uint32_t totalFrames = soundBuffer->getFrameCount();

        if( totalFrames == 0 )
        {
            return noErr;
        }

        if( renderedFrames >= frames )
        {
            return noErr;
        }

        if( m_loop.load() == true )
        {
            return noErr;
        }

        m_finished = true;

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
        if( _frequency == 0 )
        {
            return false;
        }

        if( _channels == 0 )
        {
            return false;
        }

        AudioUnitSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        AudioUnitSoundSourcePtr source = AudioUnitSoundSourcePtr::from( this );
        uint32_t busIndex = INVALID_BUS_INDEX;
        if( soundSystemExtension->acquireSourceBus( source, _frequency, _channels, _gain, &busIndex ) == false )
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
}
