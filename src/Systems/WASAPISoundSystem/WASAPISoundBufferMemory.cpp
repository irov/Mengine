#include "WASAPISoundBufferMemory.h"

#include "Interface/SoundCodecInterface.h"

#include "WASAPISoundPCMHelper.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    namespace Detail
    {
        static void resamplePCM16Linear_( const int16_t * _src, uint32_t _sourceFrames, uint32_t _channels, uint32_t _sourceFrequency, int16_t * _dst, uint32_t _targetFrames, uint32_t _targetFrequency )
        {
            if( _src == nullptr || _dst == nullptr || _sourceFrames == 0 || _channels == 0 || _sourceFrequency == 0 || _targetFrequency == 0 || _targetFrames == 0 )
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

                const int16_t * frame0 = _src + baseFrame * _channels;
                const int16_t * frame1 = _src + nextFrame * _channels;
                int16_t * targetFrame = _dst + frame * _channels;

                for( uint32_t channel = 0; channel != _channels; ++channel )
                {
                    targetFrame[channel] = Helper::interpolateWASAPISoundPCM16Sample( frame0, frame1, _channels, channel, alpha );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundBufferMemory::WASAPISoundBufferMemory()
        : m_pcmDataSize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundBufferMemory::~WASAPISoundBufferMemory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferMemory::_acquireSoundBuffer()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferMemory::_releaseSoundBuffer()
    {
        m_pcmMemory = nullptr;
        m_soundDecoder = nullptr;
        m_pcmDataSize = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferMemory::updateSoundBuffer()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferMemory::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

        m_sourceFrequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;

        if( m_channels == 0 || m_sourceFrequency == 0 )
        {
            LOGGER_WARNING( "invalid sound format frequency: %u channels: %u"
                , m_sourceFrequency
                , m_channels
            );

            return false;
        }

        size_t size = dataInfo->size;

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

        size_t sourceFrameSize = m_channels * 2;
        size_t correctDecodeSize = decodeSize - decodeSize % sourceFrameSize;

        if( correctDecodeSize == 0 )
        {
            LOGGER_WARNING( "invalid sound decode frame alignment: %zu"
                , decodeSize
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

        uint32_t sourceFrames = (uint32_t)(correctDecodeSize / sourceFrameSize);
        uint32_t targetFrames = (uint32_t)(((uint64_t)sourceFrames * (uint64_t)playbackFrequency + (uint64_t)m_sourceFrequency - 1ULL) / (uint64_t)m_sourceFrequency);

        if( targetFrames == 0 )
        {
            targetFrames = 1;
        }

        size_t targetSize = (size_t)targetFrames * sourceFrameSize;

        MemoryInterfacePtr targetMemory = Helper::createMemoryCacheBuffer( targetSize, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( targetMemory, "invalid resampled sound memory: %zu"
            , targetSize
        );

        const int16_t * sourcePCM = sourceMemory->getBuffer();
        int16_t * targetPCM = targetMemory->getBuffer();

        Detail::resamplePCM16Linear_( sourcePCM, sourceFrames, m_channels, m_sourceFrequency, targetPCM, targetFrames, playbackFrequency );

        m_frequency = playbackFrequency;
        m_pcmMemory = targetMemory;
        m_pcmDataSize = targetSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferMemory::playSource( bool _looped, float _position )
    {
        MENGINE_UNUSED( _looped );
        MENGINE_UNUSED( _position );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferMemory::stopSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferMemory::pauseSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferMemory::resumeSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferMemory::setTimePosition( float _position )
    {
        MENGINE_UNUSED( _position );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferMemory::getTimePosition( float * const _position ) const
    {
        *_position = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferMemory::setVolume( float _gain )
    {
        MENGINE_UNUSED( _gain );
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & WASAPISoundBufferMemory::getPCMMemory() const
    {
        return m_pcmMemory;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundBufferMemory::getFrameCount() const
    {
        if( m_channels == 0 )
        {
            return 0;
        }

        return (uint32_t)(m_pcmDataSize / this->getFrameSize());
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundBufferMemory::renderMixerFrames( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, uint32_t _frames, float _gain, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition )
    {
        if( _mixBuffer == nullptr || _frames == 0 || _outputChannels == 0 || m_channels == 0 )
        {
            *_outFramePosition = _framePosition;

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

            const int16_t * src = base + currentFrame * m_channels;

            for( uint32_t frame = 0; frame != copyFrames; ++frame )
            {
                const int16_t * srcFrame = src + frame * m_channels;
                float * dstFrame = _mixBuffer + (_frameOffset + renderedFrames + frame) * _outputChannels;

                for( uint32_t channel = 0; channel != _outputChannels; ++channel )
                {
                    dstFrame[channel] += Helper::resolveWASAPISoundPCM16Sample( srcFrame, m_channels, channel, _outputChannels ) * _gain;
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
