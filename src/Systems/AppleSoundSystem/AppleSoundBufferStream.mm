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

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferStream::AppleSoundBufferStream()
        : m_looped( false )
        , m_updating( false )
        , m_finished( false )
        , m_playCursorBytes( 0 )
        , m_writeCount( 0 )
        , m_readCount( 0 )
        , m_basePositionMs( 0.f )
        , m_ringBufferSizeMask( 0 )
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

        size_t requestedSize = MENGINE_APPLE_STREAM_BUFFER_SIZE * MENGINE_APPLE_STREAM_BUFFER_COUNT;

        // Round up to power of two for efficient masking (like OpenAL Soft ring buffer)
        size_t powerOfTwo = 1;
        while( powerOfTwo < requestedSize )
        {
            powerOfTwo <<= 1;
        }

        memory->newBuffer( powerOfTwo );

        m_memory = memory;
        m_ringBufferSize = powerOfTwo;
        m_ringBufferSizeMask = (uint32_t)(powerOfTwo - 1);
        m_ringBuffer = m_memory->getBuffer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::_releaseSoundBuffer()
    {
        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

            m_updating = false;
            m_finished = true;

            this->resetRingBuffer_();

            m_playCursorBytes = 0;
            m_basePositionMs = 0.f;

            m_soundDecoder = nullptr;
            m_memory = nullptr;
            m_ringBuffer = nullptr;
            m_ringBufferSize = 0;
            m_ringBufferSizeMask = 0;
        }

        m_mutexDecoder = nullptr;
        m_mutexUpdating = nullptr;
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
        if( _position > m_duration )
        {
            LOGGER_ASSERTION( "pos %f > length %f"
                , _position
                , m_duration
            );

            return false;
        }

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

            m_looped = _looped;
            m_updating = false;
            m_finished = false;
            m_basePositionMs = _position;
            m_playCursorBytes = 0;

            this->resetRingBuffer_();

            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

            if( m_soundDecoder->seek( _position ) == false )
            {
                LOGGER_ASSERTION( "invalid seek '%f'"
                    , _position
                );

                return false;
            }

            if( this->prebuffer_() == false )
            {
                return false;
            }

            LOGGER_MESSAGE( "[apple] stream prebuffer freq: %u channels: %u duration: %f position: %f readable: %u finished: %u loop: %u"
                , m_frequency
                , m_channels
                , m_duration
                , _position
                , this->getReadableBytes_()
                , m_finished.load()
                , m_looped.load()
            );

        }

        m_updating = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferStream::stopSource()
    {
        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

            m_updating = false;
            m_finished = true;
            m_basePositionMs = 0.f;
            m_playCursorBytes = 0;

            this->resetRingBuffer_();
        }
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
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

        if( m_soundDecoder->seek( _position ) == false )
        {
            return false;
        }

        this->resetRingBuffer_();

        m_basePositionMs = _position;
        m_playCursorBytes = 0;
        m_finished = false;

        if( this->prebuffer_() == false )
        {
            return false;
        }

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

        if( m_finished.load() == true && this->getReadableBytes_() == 0 )
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

        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 || m_ringBuffer == nullptr )
        {
            return true;
        }

        UInt32 renderedFrames = 0;

        // SPSC: reader loads writeCount with acquire, reads own readCount relaxed
        uint32_t w = m_writeCount.load( StdAtomic::memory_order_acquire );
        uint32_t r = m_readCount.load( StdAtomic::memory_order_relaxed );
        uint32_t readable = w - r;

        while( renderedFrames != _frames && readable != 0 )
        {
            uint32_t readIdx = r & m_ringBufferSizeMask;
            uint32_t tailBytes = (uint32_t)(m_ringBufferSize) - readIdx;
            uint32_t framesLeft = _frames - renderedFrames;
            uint32_t maxBytes = framesLeft * frameSize;

            uint32_t bytesToCopy = readable;

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

            const int16_t * srcSamples = reinterpret_cast<const int16_t *>(m_ringBuffer + readIdx);
            uint32_t copyFrames = bytesToCopy / frameSize;

            this->writeMixerFrames_( _ioData, renderedFrames, srcSamples, copyFrames );

            r += bytesToCopy;
            readable -= bytesToCopy;
            m_playCursorBytes.fetch_add( bytesToCopy, StdAtomic::memory_order_relaxed );

            renderedFrames += copyFrames;
        }

        // SPSC: reader stores readCount with release
        m_readCount.store( r, StdAtomic::memory_order_release );

        *_renderedFrames = renderedFrames;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferStream::renderMixerFrames( AudioBufferList * _ioData, uint32_t _frameOffset, uint32_t _frames, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition )
    {
        MENGINE_UNUSED( _framePosition );
        MENGINE_UNUSED( _loop );

        *_outFramePosition = _framePosition;

        UInt32 renderedFrames = 0;
        this->renderMixerFrames( _ioData, (UInt32)_frames, &renderedFrames );

        return (uint32_t)renderedFrames;
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
            // SPSC: writer loads readCount with acquire, reads own writeCount relaxed
            uint32_t r = m_readCount.load( StdAtomic::memory_order_acquire );
            uint32_t w = m_writeCount.load( StdAtomic::memory_order_relaxed );
            uint32_t readable = w - r;
            size_t freeBytes = m_ringBufferSize - readable;

            freeBytes -= freeBytes % frameSize;

            if( freeBytes < frameSize )
            {
                break;
            }

            uint32_t writeIdx = w & m_ringBufferSizeMask;
            size_t capacity = m_ringBufferSize - writeIdx;

            if( capacity > freeBytes )
            {
                capacity = freeBytes;
            }

            capacity -= capacity % frameSize;

            if( capacity == 0 )
            {
                break;
            }

            size_t written = 0;
            if( this->decodeSegment_( m_ringBuffer + writeIdx, capacity, &written ) == false )
            {
                return false;
            }

            if( written == 0 )
            {
                break;
            }

            // SPSC: writer stores writeCount with release
            m_writeCount.store( w + (uint32_t)written, StdAtomic::memory_order_release );

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
        m_writeCount.store( 0, StdAtomic::memory_order_relaxed );
        m_readCount.store( 0, StdAtomic::memory_order_relaxed );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferStream::getReadableBytes_() const
    {
        uint32_t w = m_writeCount.load( StdAtomic::memory_order_acquire );
        uint32_t r = m_readCount.load( StdAtomic::memory_order_relaxed );

        return w - r;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferStream::getWritableBytes_() const
    {
        uint32_t r = m_readCount.load( StdAtomic::memory_order_acquire );
        uint32_t w = m_writeCount.load( StdAtomic::memory_order_relaxed );

        return (uint32_t)m_ringBufferSize - (w - r);
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
}
