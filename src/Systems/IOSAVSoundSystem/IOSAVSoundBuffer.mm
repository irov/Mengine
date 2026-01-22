#import "IOSAVSoundBuffer.h"
#import "IOSAVSoundSystem.h"

#import "Kernel/MemoryStreamHelper.h"
#import "Kernel/AssertionMemoryPanic.h"
#import "Kernel/Logger.h"

#import <AVFoundation/AVFoundation.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    IOSAVSoundBuffer::IOSAVSoundBuffer()
        : m_soundSystem( nullptr )
        , m_streamable( false )
    {
        m_dataInfo = SoundCodecDataInfo();
    }
    //////////////////////////////////////////////////////////////////////////
    IOSAVSoundBuffer::~IOSAVSoundBuffer()
    {
        this->releaseSoundBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundBuffer::initialize( IOSAVSoundSystem * _soundSystem, const SoundDecoderInterfacePtr & _decoder, bool _streamable )
    {
        m_soundSystem = _soundSystem;
        m_soundDecoder = _decoder;
        m_streamable = _streamable;

        if( m_streamable == false )
        {
            this->acquireSoundBuffer();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundBuffer::acquireSoundBuffer()
    {
        if( m_memory != nullptr )
        {
            return true;
        }

        if( m_soundDecoder == nullptr )
        {
            return false;
        }

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();

        if( dataInfo == nullptr )
        {
            LOGGER_ERROR( "invalid sound decoder info" );

            return false;
        }

        m_dataInfo = *dataInfo;

        size_t size = dataInfo->size;

        MemoryInterfacePtr memory = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid sound memory" );

        void * buffer = memory->getBuffer();

        SoundDecoderData data;
        data.buffer = buffer;
        data.size = size;

        size_t decode = m_soundDecoder->decode( &data );

        if( decode == 0 )
        {
            LOGGER_ERROR( "failed decode sound" );

            return false;
        }

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundBuffer::releaseSoundBuffer()
    {
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundBuffer::updateSoundBuffer()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & IOSAVSoundBuffer::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundCodecDataInfo & IOSAVSoundBuffer::getDataInfo() const
    {
        return m_dataInfo;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & IOSAVSoundBuffer::getMemory() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundBuffer::isStreamable() const
    {
        return m_streamable;
    }
    //////////////////////////////////////////////////////////////////////////
    void * IOSAVSoundBuffer::createNativePCMBuffer( float _startPositionMs ) const
    {
        if( m_memory == nullptr )
        {
            return nullptr;
        }

        AVAudioFormat * format = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatInt16 sampleRate:m_dataInfo.frequency channels:m_dataInfo.channels interleaved:YES];

        if( format == nil )
        {
            LOGGER_ERROR( "invalid create audio format" );

            return nullptr;
        }

        UInt32 bytesPerFrame = format.streamDescription->mBytesPerFrame;

        if( bytesPerFrame == 0 )
        {
            LOGGER_ERROR( "invalid bytes per frame" );

            return nullptr;
        }

        AVAudioFrameCount frameCapacity = (AVAudioFrameCount)(m_dataInfo.size / bytesPerFrame);

        AVAudioPCMBuffer * buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:format frameCapacity:frameCapacity];

        if( buffer == nil )
        {
            LOGGER_ERROR( "invalid create audio pcm buffer" );

            return nullptr;
        }

        AudioBufferList * audioBufferList = buffer.audioBufferList;

        if( audioBufferList->mNumberBuffers == 0 )
        {
            LOGGER_ERROR( "invalid audio buffer list" );

            return nullptr;
        }

        AudioBuffer & audioBuffer = audioBufferList->mBuffers[0];

        uint8_t * memoryBuffer = static_cast<uint8_t *>( m_memory->getBuffer() );

        uint32_t startFrame = (uint32_t)((_startPositionMs / 1000.f) * m_dataInfo.frequency);
        uint32_t bytesPerFrame = format.streamDescription->mBytesPerFrame;
        uint32_t startOffset = startFrame * bytesPerFrame;

        if( startOffset >= m_dataInfo.size )
        {
            startOffset = (uint32_t)m_dataInfo.size;
        }

        uint32_t copySize = (uint32_t)m_dataInfo.size - startOffset;

        if( copySize == 0 )
        {
            return nullptr;
        }

        memcpy( audioBuffer.mData, memoryBuffer + startOffset, copySize );

        audioBuffer.mDataByteSize = copySize;

        AVAudioFrameCount frameLength = copySize / bytesPerFrame;

        buffer.frameLength = frameLength;

        return (__bridge_retained void *)buffer;
    }
    //////////////////////////////////////////////////////////////////////////
}
