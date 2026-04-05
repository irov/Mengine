#include "AppleSoundSource.h"

#include "AppleSoundSystemExtensionInterface.h"

#include "AppleSoundBufferBase.h"
#include "AppleSoundBufferMemory.h"
#include "AppleSoundBufferStream.h"
#include "AppleSoundPCMHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/SpinLockScope.h"

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
        static void writeMemoryFrames( AudioBufferList * const _ioData, UInt32 _frameOffset, const int16_t * _src, uint32_t _sourceChannels, UInt32 _frames )
        {
            if( _ioData == nullptr || _src == nullptr || _sourceChannels == 0 || _frames == 0 )
            {
                return;
            }

            UInt32 bufferCount = _ioData->mNumberBuffers;

            if( bufferCount == 0 )
            {
                return;
            }

            if( bufferCount == 1 )
            {
                AudioBuffer & buffer = _ioData->mBuffers[0];

                if( buffer.mData == nullptr || buffer.mNumberChannels == 0 )
                {
                    return;
                }

                UInt32 destinationChannels = buffer.mNumberChannels;
                Float32 * dst = static_cast<Float32 *>( buffer.mData ) + _frameOffset * destinationChannels;

                for( UInt32 frame = 0; frame != _frames; ++frame )
                {
                    const int16_t * srcFrame = _src + frame * _sourceChannels;

                    for( UInt32 channel = 0; channel != destinationChannels; ++channel )
                    {
                        dst[frame * destinationChannels + channel] = Helper::resolveAppleSoundPCM16Sample( srcFrame, _sourceChannels, channel, destinationChannels );
                    }
                }

                return;
            }

            for( UInt32 channel = 0; channel != bufferCount; ++channel )
            {
                AudioBuffer & buffer = _ioData->mBuffers[channel];

                if( buffer.mData == nullptr )
                {
                    continue;
                }

                Float32 * dst = static_cast<Float32 *>( buffer.mData ) + _frameOffset;

                for( UInt32 frame = 0; frame != _frames; ++frame )
                {
                    const int16_t * srcFrame = _src + frame * _sourceChannels;

                    dst[frame] = Helper::resolveAppleSoundPCM16Sample( srcFrame, _sourceChannels, channel, bufferCount );
                }
            }
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

            AppleSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
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

        AppleSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
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

        m_playing = false;
        m_pausing = false;
        m_finished = false;

        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer != nullptr )
        {
            soundBuffer->stopSource();
        }

        this->releaseSourceBus_();

        m_framePosition = 0;
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
                AppleSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
                    ->getUnknown();

                soundSystemExtension->setSourceBusVolume( busIndex, gain );
            }
        }
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

        if( m_playing.load() == false && m_pausing.load() == false )
        {
            AppleSoundBufferMemoryPtr memoryBuffer = AppleSoundBufferMemoryPtr::from( soundBuffer );

            if( memoryBuffer != nullptr )
            {
                float position = _position;

                if( position < 0.f )
                {
                    position = 0.f;
                }

                uint32_t frequency = memoryBuffer->getFrequency();
                uint32_t positionFrame = (uint32_t)(position * (float)frequency / 1000.f);

                if( positionFrame > memoryBuffer->getFrameCount() )
                {
                    positionFrame = memoryBuffer->getFrameCount();
                }

                m_framePosition = positionFrame;

                return true;
            }

            AppleSoundBufferStreamPtr streamBuffer = AppleSoundBufferStreamPtr::from( soundBuffer );

            if( streamBuffer != nullptr )
            {
                return streamBuffer->setTimePosition( _position );
            }

            return true;
        }

        float position = _position;

        float total = soundBuffer->getTimeDuration();

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

        AppleSoundBufferMemoryPtr memoryBuffer = AppleSoundBufferMemoryPtr::from( soundBuffer );

        if( memoryBuffer != nullptr )
        {
            uint32_t frequency = memoryBuffer->getFrequency();
            uint32_t positionFrame = (uint32_t)(position * (float)frequency / 1000.f);

            if( positionFrame > memoryBuffer->getFrameCount() )
            {
                positionFrame = memoryBuffer->getFrameCount();
            }

            m_framePosition = positionFrame;

            return true;
        }

        AppleSoundBufferStreamPtr streamBuffer = AppleSoundBufferStreamPtr::from( soundBuffer );

        if( streamBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid buffer type for position update" );

            return false;
        }

        if( streamBuffer->setTimePosition( position ) == false )
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

        AppleSoundBufferMemoryPtr memoryBuffer = AppleSoundBufferMemoryPtr::from( soundBuffer );

        if( memoryBuffer != nullptr )
        {
            uint32_t frequency = memoryBuffer->getFrequency();

            if( frequency == 0 )
            {
                return 0.f;
            }

            float position = (float)m_framePosition.load() * 1000.f / (float)frequency;

            if( position > memoryBuffer->getTimeDuration() )
            {
                position = memoryBuffer->getTimeDuration();
            }

            return position;
        }

        float position = 0.f;

        AppleSoundBufferStreamPtr streamBuffer = AppleSoundBufferStreamPtr::from( soundBuffer );

        if( streamBuffer == nullptr )
        {
            LOGGER_ASSERTION( "invalid sound buffer type" );

            return 0.f;
        }

        if( streamBuffer->getTimePosition( &position ) == false )
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

        if( m_playing.load() == false || m_pausing.load() == true )
        {
            return noErr;
        }

        AppleSoundBufferBasePtr soundBuffer = this->getSoundBuffer_();

        if( soundBuffer == nullptr )
        {
            return noErr;
        }

        AppleSoundBufferMemoryPtr memoryBuffer = AppleSoundBufferMemoryPtr::from( soundBuffer );

        if( memoryBuffer != nullptr )
        {
            uint32_t channels = memoryBuffer->getChannels();
            uint32_t totalFrames = memoryBuffer->getFrameCount();
            uint32_t currentFrame = m_framePosition.load();
            uint32_t framesLeft = _frames;
            UInt32 frameOffset = 0;

            while( framesLeft != 0 )
            {
                if( currentFrame >= totalFrames )
                {
                    if( m_loop.load() == true && totalFrames != 0 )
                    {
                        currentFrame = 0;
                    }
                    else
                    {
                        break;
                    }
                }

                uint32_t copyFrames = totalFrames - currentFrame;

                if( copyFrames > framesLeft )
                {
                    copyFrames = framesLeft;
                }
                
                const MemoryInterfacePtr & memory = memoryBuffer->getPCMMemory();
                
                const int16_t * base = memory->getBuffer();

                const int16_t * src = base + currentFrame * channels;

                Detail::writeMemoryFrames( _ioData, frameOffset, src, channels, copyFrames );

                currentFrame += copyFrames;
                frameOffset += copyFrames;
                framesLeft -= copyFrames;
            }

            m_framePosition = currentFrame;

            if( currentFrame >= totalFrames && m_loop.load() == false )
            {
                m_finished = true;
            }

            return noErr;
        }

        AppleSoundBufferStreamPtr streamBuffer = AppleSoundBufferStreamPtr::from( soundBuffer );

        if( streamBuffer != nullptr )
        {
            UInt32 renderedFrames = 0;
            streamBuffer->renderMixerFrames( _ioData, _frames, &renderedFrames );
        }

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
}
