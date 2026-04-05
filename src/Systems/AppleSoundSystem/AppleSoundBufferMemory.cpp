#include "AppleSoundBufferMemory.h"

#include "Interface/SoundCodecInterface.h"

#include "AppleSoundPCMHelper.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferMemory::AppleSoundBufferMemory()
        : m_pcmDataSize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferMemory::~AppleSoundBufferMemory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::_acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::_releaseSoundBuffer()
    {
        m_pcmMemory = nullptr;
        m_soundDecoder = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::updateSoundBuffer()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

        m_frequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;
        size_t size = dataInfo->size;

        MemoryInterfacePtr pcmMemory = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( pcmMemory, "invalid sound memory: %zu"
            , size
        );

        void * pcmBuffer = pcmMemory->getBuffer();

        SoundDecoderData data;
        data.buffer = pcmBuffer;
        data.size = size;

        size_t decodeSize = _soundDecoder->decode( &data );

        if( decodeSize == 0 )
        {
            LOGGER_WARNING( "invalid sound decode: %zu"
                , size
            );

            return false;
        }

        switch( m_channels )
        {
        case 1:
            {
                m_isStereo = false;
            }break;
        case 2:
            {
                m_isStereo = true;
            }break;
        default:
            {
                LOGGER_WARNING( "invalid sound format channels: %u"
                    , m_channels
                );

                return false;
            }break;
        }

        size_t frameSize = m_channels * 2;
        size_t correctDecodeSize = decodeSize - decodeSize % frameSize;

        m_soundDecoder = _soundDecoder;
        m_pcmMemory = pcmMemory;
        m_pcmDataSize = correctDecodeSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::playSource( bool _looped, float _position )
    {
        MENGINE_UNUSED( _looped );
        MENGINE_UNUSED( _position );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::stopSource()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::pauseSource()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::resumeSource()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::setTimePosition( float _position )
    {
        MENGINE_UNUSED( _position );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferMemory::getTimePosition( float * const _position ) const
    {
        *_position = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferMemory::setVolume( float _gain )
    {
        MENGINE_UNUSED( _gain );
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & AppleSoundBufferMemory::getPCMMemory() const
    {
        return m_pcmMemory;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferMemory::getFrameCount() const
    {
        if( m_channels == 0 )
        {
            return 0;
        }

        return (uint32_t)(m_pcmDataSize / this->getFrameSize());
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferMemory::renderMixerFrames( AudioBufferList * _ioData, uint32_t _frameOffset, uint32_t _frames, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition )
    {
        *_outFramePosition = _framePosition;

        if( _ioData == nullptr || _frames == 0 || m_channels == 0 )
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

        UInt32 bufferCount = _ioData->mNumberBuffers;

        if( bufferCount == 0 )
        {
            return 0;
        }

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

            if( bufferCount == 1 )
            {
                AudioBuffer & buffer = _ioData->mBuffers[0];

                if( buffer.mData != nullptr && buffer.mNumberChannels != 0 )
                {
                    UInt32 destinationChannels = buffer.mNumberChannels;
                    Float32 * dst = static_cast<Float32 *>( buffer.mData ) + (_frameOffset + renderedFrames) * destinationChannels;

                    for( uint32_t frame = 0; frame != copyFrames; ++frame )
                    {
                        const int16_t * srcFrame = src + frame * m_channels;

                        for( UInt32 channel = 0; channel != destinationChannels; ++channel )
                        {
                            dst[frame * destinationChannels + channel] = Helper::resolveAppleSoundPCM16Sample( srcFrame, m_channels, channel, destinationChannels );
                        }
                    }
                }
            }
            else
            {
                for( UInt32 channel = 0; channel != bufferCount; ++channel )
                {
                    AudioBuffer & buffer = _ioData->mBuffers[channel];

                    if( buffer.mData == nullptr )
                    {
                        continue;
                    }

                    Float32 * dst = static_cast<Float32 *>( buffer.mData ) + (_frameOffset + renderedFrames);

                    for( uint32_t frame = 0; frame != copyFrames; ++frame )
                    {
                        const int16_t * srcFrame = src + frame * m_channels;

                        dst[frame] = Helper::resolveAppleSoundPCM16Sample( srcFrame, m_channels, channel, bufferCount );
                    }
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
