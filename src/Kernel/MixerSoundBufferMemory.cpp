#include "MixerSoundBufferMemory.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/MixerSoundPCMHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdLimits.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void resamplePCM16Linear( const int16_t * _src, uint32_t _sourceFrames, uint32_t _channels, uint32_t _sourceFrequency, int16_t * _dst, uint32_t _targetFrames, uint32_t _targetFrequency )
        {
            if( _src == nullptr )
            {
                return;
            }

            if( _dst == nullptr )
            {
                return;
            }

            if( _sourceFrames == 0 )
            {
                return;
            }

            if( _channels == 0 )
            {
                return;
            }

            if( _sourceFrequency == 0 )
            {
                return;
            }

            if( _targetFrequency == 0 )
            {
                return;
            }

            if( _targetFrames == 0 )
            {
                return;
            }

            double step = (double)_sourceFrequency / (double)_targetFrequency;

            for( uint32_t frame = 0; frame != _targetFrames; ++frame )
            {
                double sourcePosition = (double)frame * step;
                uint32_t baseFrame = (uint32_t)sourcePosition;

                if( baseFrame >= _sourceFrames )
                {
                    baseFrame = _sourceFrames - 1;
                }

                uint32_t nextFrame = baseFrame + 1;

                if( nextFrame >= _sourceFrames )
                {
                    nextFrame = _sourceFrames - 1;
                }

                float alpha = (float)(sourcePosition - (double)baseFrame);

                const int16_t * frame0 = _src + (size_t)baseFrame * _channels;
                const int16_t * frame1 = _src + (size_t)nextFrame * _channels;
                int16_t * targetFrame = _dst + (size_t)frame * _channels;

                for( uint32_t channel = 0; channel != _channels; ++channel )
                {
                    targetFrame[channel] = Helper::interpolateMixerSoundPCM16Sample( frame0, frame1, _channels, channel, alpha );
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MixerSoundBufferMemory::MixerSoundBufferMemory()
        : m_pcmDataSize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MixerSoundBufferMemory::~MixerSoundBufferMemory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferMemory::_acquireSoundBuffer()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferMemory::_releaseSoundBuffer()
    {
        m_pcmMemory = nullptr;
        m_soundDecoder = nullptr;
        m_pcmDataSize = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferMemory::updateSoundBuffer()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferMemory::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

        m_sourceFrequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;

        if( m_channels == 0 )
        {
            LOGGER_WARNING( "invalid PCM16 sound channels: %u"
                , m_channels
            );

            return false;
        }

        if( m_channels > 2 )
        {
            LOGGER_WARNING( "invalid PCM16 sound channels: %u"
                , m_channels
            );

            return false;
        }

        if( m_sourceFrequency == 0 )
        {
            LOGGER_WARNING( "invalid PCM16 sound frequency: %u"
                , m_sourceFrequency
            );

            return false;
        }

        if( dataInfo->bits != sizeof( int16_t ) )
        {
            LOGGER_WARNING( "invalid PCM16 sound sample bytes: %u"
                , dataInfo->bits
            );

            return false;
        }

        size_t size = dataInfo->size;

        if( size < (size_t)m_channels * sizeof( int16_t ) )
        {
            LOGGER_WARNING( "invalid sound data size: %zu"
                , size
            );

            return false;
        }

        MemoryInterfacePtr sourceMemory = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( sourceMemory, "invalid sound memory: %zu"
            , size
        );

        void * sourceBuffer = sourceMemory->getBuffer();

        SoundDecoderData data;
        data.buffer = sourceBuffer;
        data.size = size;

        size_t decodeSize = _soundDecoder->decode( &data );

        if( decodeSize == 0 )
        {
            LOGGER_WARNING( "invalid sound decode: %zu"
                , size
            );

            return false;
        }

        size_t sourceFrameSize = m_channels * sizeof( int16_t );
        size_t correctDecodeSize = decodeSize - decodeSize % sourceFrameSize;

        if( correctDecodeSize == 0 )
        {
            LOGGER_WARNING( "invalid sound decode frame alignment: %zu"
                , decodeSize
            );

            return false;
        }

        size_t sourceFrameCount = correctDecodeSize / sourceFrameSize;
        uint32_t maxFrameCount = (StdLimits::numeric_limits<uint32_t>::max)();

        if( sourceFrameCount > maxFrameCount )
        {
            LOGGER_WARNING( "sound frame count exceeds mixer limit: %zu"
                , sourceFrameCount
            );

            return false;
        }

        uint32_t playbackFrequency = m_outputFrequency != 0 ? m_outputFrequency : m_sourceFrequency;

        m_isStereo = m_channels > 1;
        m_soundDecoder = _soundDecoder;

        if( playbackFrequency == m_sourceFrequency )
        {
            m_frequency = m_sourceFrequency;
            m_pcmMemory = sourceMemory;
            m_pcmDataSize = correctDecodeSize;

            return true;
        }

        uint32_t sourceFrames = (uint32_t)sourceFrameCount;
        uint64_t targetFrameCount = ((uint64_t)sourceFrames * playbackFrequency + m_sourceFrequency - 1) / m_sourceFrequency;

        if( targetFrameCount > maxFrameCount )
        {
            LOGGER_WARNING( "resampled sound frame count exceeds mixer limit: %llu"
                , (unsigned long long)targetFrameCount
            );

            return false;
        }

        size_t maxDataSize = (StdLimits::numeric_limits<size_t>::max)();

        if( targetFrameCount > maxDataSize / sourceFrameSize )
        {
            LOGGER_WARNING( "resampled sound data size exceeds memory limit" );

            return false;
        }

        uint32_t targetFrames = (uint32_t)targetFrameCount;
        size_t targetSize = (size_t)targetFrames * sourceFrameSize;

        MemoryInterfacePtr targetMemory = Helper::createMemoryCacheBuffer( targetSize, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( targetMemory, "invalid resampled sound memory: %zu"
            , targetSize
        );

        const int16_t * sourcePCM = sourceMemory->getBuffer();
        int16_t * targetPCM = targetMemory->getBuffer();

        Detail::resamplePCM16Linear( sourcePCM, sourceFrames, m_channels, m_sourceFrequency, targetPCM, targetFrames, playbackFrequency );

        m_frequency = playbackFrequency;
        m_pcmMemory = targetMemory;
        m_pcmDataSize = targetSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferMemory::playSource( bool _looped, float _position )
    {
        MENGINE_UNUSED( _looped );
        MENGINE_UNUSED( _position );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferMemory::stopSource()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferMemory::pauseSource()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferMemory::resumeSource()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferMemory::setTimePosition( float _position )
    {
        MENGINE_UNUSED( _position );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferMemory::getTimePosition( float * const _position ) const
    {
        *_position = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferMemory::setVolume( float _gain )
    {
        MENGINE_UNUSED( _gain );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & MixerSoundBufferMemory::getPCMMemory() const
    {
        return m_pcmMemory;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundBufferMemory::getFrameCount() const
    {
        if( m_channels == 0 )
        {
            return 0;
        }

        uint32_t frameSize = this->getFrameSize();
        uint32_t frameCount = (uint32_t)(m_pcmDataSize / frameSize);

        return frameCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundBufferMemory::renderMixerFrames( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, uint32_t _frames, float _gain, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition )
    {
        *_outFramePosition = _framePosition;

        if( _mixBuffer == nullptr )
        {
            return 0;
        }

        if( _frames == 0 )
        {
            return 0;
        }

        if( _outputChannels == 0 )
        {
            return 0;
        }

        if( m_channels == 0 )
        {
            return 0;
        }

        uint32_t totalFrames = this->getFrameCount();

        if( totalFrames == 0 )
        {
            *_outFramePosition = 0;

            return 0;
        }

        const int16_t * base = m_pcmMemory->getBuffer();

        uint32_t currentFrame = _framePosition;
        uint32_t framesLeft = _frames;
        uint32_t renderedFrames = 0;

        while( framesLeft != 0 )
        {
            if( currentFrame >= totalFrames )
            {
                if( _loop == true )
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

            const int16_t * src = base + (size_t)currentFrame * m_channels;

            for( uint32_t frame = 0; frame != copyFrames; ++frame )
            {
                const int16_t * srcFrame = src + frame * m_channels;
                float * dstFrame = _mixBuffer + (_frameOffset + renderedFrames + frame) * _outputChannels;

                for( uint32_t channel = 0; channel != _outputChannels; ++channel )
                {
                    float sample = Helper::resolveMixerSoundPCM16Sample( srcFrame, m_channels, channel, _outputChannels );
                    dstFrame[channel] += sample * _gain;
                }
            }

            currentFrame += copyFrames;
            renderedFrames += copyFrames;
            framesLeft -= copyFrames;
        }

        *_outFramePosition = currentFrame;

        return renderedFrames;
    }
    //////////////////////////////////////////////////////////////////////////
}
