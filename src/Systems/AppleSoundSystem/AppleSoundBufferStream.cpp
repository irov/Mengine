#include "AppleSoundBufferStream.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "AppleSoundPCMHelper.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"
#include "Config/StdThread.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferStream::AppleSoundBufferStream()
        : m_looped( false )
        , m_updating( false )
        , m_finished( false )
        , m_renderBarrier( false )
        , m_readOffset( 0 )
        , m_writeOffset( 0 )
        , m_availableBytes( 0 )
        , m_playCursorBytes( 0 )
        , m_activeRenders( 0 )
        , m_loggedRenderLayout( false )
        , m_loggedUnderflow( false )
        , m_basePositionMs( 0.f )
        , m_ringBufferSize( 0 )
        , m_ringBuffer( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferStream::~AppleSoundBufferStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::_acquireSoundBuffer()
    {
        ThreadMutexInterfacePtr mutexDecoder = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexDecoder, "invalid create decoder mutex" );

        m_mutexDecoder = mutexDecoder;

        ThreadMutexInterfacePtr mutexUpdating = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexUpdating, "invalid create mutex" );

        m_mutexUpdating = mutexUpdating;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        size_t totalSize = MENGINE_APPLE_STREAM_BUFFER_SIZE * MENGINE_APPLE_STREAM_BUFFER_COUNT;
        memory->newBuffer( totalSize );

        m_memory = memory;
        m_ringBufferSize = totalSize;
        m_ringBuffer = m_memory->getBuffer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::_releaseSoundBuffer()
    {
        this->beginMutableState_();

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

            m_updating = false;
            m_finished = true;
            m_readOffset = 0;
            m_writeOffset = 0;
            m_availableBytes = 0;
            m_playCursorBytes = 0;
            m_basePositionMs = 0.f;

            m_soundDecoder = nullptr;
            m_memory = nullptr;
            m_ringBuffer = nullptr;
            m_ringBufferSize = 0;
        }

        m_mutexDecoder = nullptr;
        m_mutexUpdating = nullptr;

        this->endMutableState_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();
        m_frequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;

        if( m_channels == 1 )
        {
            m_isStereo = false;
        }
        else if( m_channels == 2 )
        {
            m_isStereo = true;
        }
        else
        {
            LOGGER_ERROR( "invalid channels %u"
                , m_channels
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::playSource( bool _looped, float _position )
    {
        this->beginMutableState_();

        if( _position > m_duration )
        {
            LOGGER_ASSERTION( "pos %f > length %f"
                , _position
                , m_duration
            );

            this->endMutableState_();

            return false;
        }

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

            m_looped = _looped;
            m_updating = false;
            m_finished = false;
            m_loggedRenderLayout = false;
            m_loggedUnderflow = false;
            m_basePositionMs = _position;
            m_playCursorBytes = 0;

            this->resetRingBuffer_();

            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

            if( m_soundDecoder->seek( _position ) == false )
            {
                LOGGER_ASSERTION( "invalid seek '%f'"
                    , _position
                );

                this->endMutableState_();

                return false;
            }

            if( this->prebuffer_() == false )
            {
                this->endMutableState_();

                return false;
            }

            LOGGER_MESSAGE( "[apple] stream prebuffer freq: %u channels: %u duration: %f position: %f available: %u finished: %u loop: %u"
                , m_frequency
                , m_channels
                , m_duration
                , _position
                , m_availableBytes.load()
                , m_finished.load()
                , m_looped.load()
            );
        }

        m_updating = true;

        this->endMutableState_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::stopSource()
    {
        this->beginMutableState_();

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

            m_updating = false;
            m_finished = true;
            m_loggedRenderLayout = false;
            m_loggedUnderflow = false;
            m_basePositionMs = 0.f;
            m_playCursorBytes = 0;

            this->resetRingBuffer_();
        }

        this->endMutableState_();
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::pauseSource()
    {
        m_updating = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::resumeSource()
    {
        m_updating = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::setTimePosition( float _position )
    {
        this->beginMutableState_();

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

        if( m_soundDecoder->seek( _position ) == false )
        {
            this->endMutableState_();

            return false;
        }

        this->resetRingBuffer_();

        m_basePositionMs = _position;
        m_playCursorBytes = 0;
        m_finished = false;
        m_loggedRenderLayout = false;
        m_loggedUnderflow = false;

        if( this->prebuffer_() == false )
        {
            this->endMutableState_();

            return false;
        }

        this->endMutableState_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::getTimePosition( float * const _position ) const
    {
        uint32_t bytesPerSecond = m_frequency * m_channels * 2;

        if( bytesPerSecond == 0 )
        {
            *_position = 0.f;

            return true;
        }

        float playedMs = (float)m_playCursorBytes.load() * 1000.f / (float)bytesPerSecond;
        float position = m_basePositionMs.load() + playedMs;

        if( m_looped.load() == true && m_duration > 0.f )
        {
            position = StdMath::fmodf( position, m_duration );
        }
        else if( position > m_duration )
        {
            position = m_duration;
        }

        *_position = position;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::updateSoundBuffer()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

        if( m_updating.load() == false )
        {
            return true;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

        if( this->prebuffer_() == false )
        {
            return false;
        }

        if( m_finished.load() == true && m_availableBytes.load() == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::setVolume( float _gain )
    {
        MENGINE_UNUSED( _gain );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::renderMixerFrames( AudioBufferList * _ioData, UInt32 _frames, UInt32 * const _renderedFrames )
    {
        *_renderedFrames = 0;

        if( _ioData == nullptr || _frames == 0 )
        {
            return true;
        }

        if( this->tryEnterRender_() == false )
        {
            return true;
        }

        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 || m_ringBuffer == nullptr )
        {
            this->leaveRender_();

            return true;
        }

        UInt32 renderedFrames = 0;

        if( m_loggedRenderLayout.exchange( true ) == false )
        {
            UInt32 bufferCount = _ioData->mNumberBuffers;
            UInt32 buffer0Channels = bufferCount > 0 ? _ioData->mBuffers[0].mNumberChannels : 0;
            UInt32 buffer0Size = bufferCount > 0 ? _ioData->mBuffers[0].mDataByteSize : 0;
            UInt32 buffer1Channels = bufferCount > 1 ? _ioData->mBuffers[1].mNumberChannels : 0;
            UInt32 buffer1Size = bufferCount > 1 ? _ioData->mBuffers[1].mDataByteSize : 0;

            LOGGER_MESSAGE( "[apple] stream render layout freq: %u channels: %u frames: %u buffers: %u b0[ch:%u size:%u] b1[ch:%u size:%u]"
                , m_frequency
                , m_channels
                , _frames
                , bufferCount
                , buffer0Channels
                , buffer0Size
                , buffer1Channels
                , buffer1Size
            );
        }

        while( renderedFrames != _frames )
        {
            uint32_t availableBytes = m_availableBytes.load();

            if( availableBytes == 0 )
            {
                break;
            }

            uint32_t readOffset = m_readOffset.load();
            uint32_t tailBytes = (uint32_t)(m_ringBufferSize - readOffset);
            uint32_t bytesToCopy = availableBytes;
            uint32_t framesLeft = _frames - renderedFrames;
            uint32_t maxBytes = framesLeft * frameSize;

            if( bytesToCopy > maxBytes )
            {
                bytesToCopy = maxBytes;
            }

            if( bytesToCopy > tailBytes )
            {
                bytesToCopy = tailBytes;
            }

            bytesToCopy -= bytesToCopy % frameSize;

            if( bytesToCopy == 0 )
            {
                break;
            }

            const int16_t * srcSamples = reinterpret_cast<const int16_t *>(m_ringBuffer + readOffset);
            uint32_t copyFrames = bytesToCopy / frameSize;

            this->writeMixerFrames_( _ioData, renderedFrames, srcSamples, copyFrames );

            m_readOffset.store( (readOffset + bytesToCopy) % (uint32_t)m_ringBufferSize );
            m_availableBytes.fetch_sub( bytesToCopy );
            m_playCursorBytes.fetch_add( bytesToCopy );

            renderedFrames += copyFrames;
        }

        *_renderedFrames = renderedFrames;

        if( renderedFrames == 0 && m_finished.load() == false && m_availableBytes.load() == 0 && m_loggedUnderflow.exchange( true ) == false )
        {
            LOGGER_WARNING( "[apple] stream render underflow freq: %u channels: %u frames: %u position: %f"
                , m_frequency
                , m_channels
                , _frames
                , m_basePositionMs.load()
            );
        }

        this->leaveRender_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::prebuffer_()
    {
        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 )
        {
            return false;
        }

        for( ;; )
        {
            uint32_t availableBytes = m_availableBytes.load();
            size_t freeBytes = m_ringBufferSize - availableBytes;

            freeBytes -= freeBytes % frameSize;

            if( freeBytes < frameSize )
            {
                break;
            }

            uint32_t writeOffset = m_writeOffset.load();
            size_t capacity = m_ringBufferSize - writeOffset;

            if( capacity > freeBytes )
            {
                capacity = freeBytes;
            }

            capacity -= capacity % frameSize;

            if( capacity == 0 )
            {
                m_writeOffset = 0;

                continue;
            }

            size_t written = 0;
            if( this->decodeSegment_( m_ringBuffer + writeOffset, capacity, &written ) == false )
            {
                return false;
            }

            if( written == 0 )
            {
                break;
            }

            m_writeOffset.store( (writeOffset + (uint32_t)written) % (uint32_t)m_ringBufferSize );
            m_availableBytes.fetch_add( (uint32_t)written );

            if( written < capacity )
            {
                break;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::decodeSegment_( uint8_t * _buffer, size_t _capacity, size_t * const _written )
    {
        size_t totalWritten = 0;

        while( totalWritten != _capacity )
        {
            SoundDecoderData decoderData;
            decoderData.buffer = _buffer + totalWritten;
            decoderData.size = _capacity - totalWritten;

            size_t bytesWritten = m_soundDecoder->decode( &decoderData );

            if( bytesWritten == 0 )
            {
                if( m_looped.load() == true )
                {
                    if( m_soundDecoder->rewind() == false )
                    {
                        return false;
                    }

                    continue;
                }

                m_finished = true;

                break;
            }

            totalWritten += bytesWritten;

            if( bytesWritten != decoderData.size )
            {
                if( m_looped.load() == true )
                {
                    if( m_soundDecoder->rewind() == false )
                    {
                        return false;
                    }

                    continue;
                }

                m_finished = true;

                break;
            }
        }

        *_written = totalWritten;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::resetRingBuffer_()
    {
        m_readOffset = 0;
        m_writeOffset = 0;
        m_availableBytes = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::writeMixerFrames_( AudioBufferList * _ioData, uint32_t _frameOffset, const int16_t * _src, uint32_t _frames ) const
    {
        if( _ioData == nullptr || _src == nullptr || m_channels == 0 || _frames == 0 )
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

            for( uint32_t frame = 0; frame != _frames; ++frame )
            {
                const int16_t * srcFrame = _src + frame * m_channels;

                for( UInt32 channel = 0; channel != destinationChannels; ++channel )
                {
                    dst[frame * destinationChannels + channel] = Helper::resolveAppleSoundPCM16Sample( srcFrame, m_channels, channel, destinationChannels );
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

            for( uint32_t frame = 0; frame != _frames; ++frame )
            {
                const int16_t * srcFrame = _src + frame * m_channels;

                dst[frame] = Helper::resolveAppleSoundPCM16Sample( srcFrame, m_channels, channel, bufferCount );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::beginMutableState_()
    {
        m_renderBarrier = true;

        while( m_activeRenders.load() != 0 )
        {
            StdThread::yield();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::endMutableState_()
    {
        m_renderBarrier = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferStream::tryEnterRender_()
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
    void AppleSoundBufferStream::leaveRender_()
    {
        m_activeRenders.fetch_sub( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
}
