#include "AudioUnitSoundBufferStream.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "AudioUnitSoundPCMHelper.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdBit.h"
#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AudioUnitSoundBufferStream::AudioUnitSoundBufferStream()
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
    AudioUnitSoundBufferStream::~AudioUnitSoundBufferStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferStream::_acquireSoundBuffer()
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

        size_t powerOfTwo = StdBit::bit_ceil( requestedSize );

        memory->newBuffer( powerOfTwo );

        m_memory = memory;
        m_ringBufferSize = powerOfTwo;
        m_ringBufferSizeMask = (uint32_t)(powerOfTwo - 1);
        m_ringBuffer = m_memory->getBuffer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundBufferStream::_releaseSoundBuffer()
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
    bool AudioUnitSoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
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
    bool AudioUnitSoundBufferStream::playSource( bool _looped, float _position )
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

            m_basePositionMs.store( _position, StdAtomic::memory_order_relaxed );
            m_playCursorBytes.store( 0, StdAtomic::memory_order_relaxed );

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
    void AudioUnitSoundBufferStream::stopSource()
    {
        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

            m_updating = false;
            m_finished = true;
            m_playCursorBytes = 0;
            m_basePositionMs = 0.f;

            this->resetRingBuffer_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundBufferStream::pauseSource()
    {
        m_updating = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundBufferStream::resumeSource()
    {
        m_updating = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferStream::setTimePosition( float _position )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

        if( m_soundDecoder->seek( _position ) == false )
        {
            return false;
        }

        this->resetRingBuffer_();

        m_finished = false;

        if( this->prebuffer_() == false )
        {
            return false;
        }

        m_basePositionMs.store( _position, StdAtomic::memory_order_relaxed );
        m_playCursorBytes.store( 0, StdAtomic::memory_order_relaxed );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferStream::getTimePosition( float * const _position ) const
    {
        uint32_t bytesPerSecond = m_frequency * m_channels * 2;

        if( bytesPerSecond == 0 )
        {
            *_position = 0.f;

            return true;
        }

        uint32_t cursorBytes = m_playCursorBytes.load( StdAtomic::memory_order_relaxed );
        float position = m_basePositionMs.load( StdAtomic::memory_order_relaxed ) + (float)cursorBytes * 1000.f / (float)bytesPerSecond;

        if( m_looped.load() == true )
        {
            if( m_duration > 0.f )
            {
                *_position = StdMath::fmodf( position, m_duration );

                return true;
            }
        }

        if( position > m_duration )
        {
            position = m_duration;
        }

        *_position = position;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferStream::updateSoundBuffer()
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

        if( m_finished.load() == false )
        {
            return true;
        }

        if( this->getReadableBytes_() != 0 )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundBufferStream::setVolume( float _gain )
    {
        MENGINE_UNUSED( _gain );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferStream::renderMixerFrames( AudioBufferList * _ioData, UInt32 _frames, UInt32 * const _renderedFrames )
    {
        *_renderedFrames = 0;

        if( _ioData == nullptr )
        {
            return true;
        }

        if( _frames == 0 )
        {
            return true;
        }

        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 )
        {
            return true;
        }

        if( m_ringBuffer == nullptr )
        {
            return true;
        }

        UInt32 renderedFrames = 0;

        // SPSC: reader loads writeCount with acquire, reads own readCount relaxed
        uint32_t writeCount = m_writeCount.load( StdAtomic::memory_order_acquire );
        uint32_t readCount = m_readCount.load( StdAtomic::memory_order_relaxed );
        uint32_t readable = writeCount - readCount;

        while( renderedFrames != _frames )
        {
            if( readable == 0 )
            {
                break;
            }

            uint32_t readIndex = readCount & m_ringBufferSizeMask;
            uint32_t tailBytes = (uint32_t)(m_ringBufferSize) - readIndex;
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

            const int16_t * srcSamples = reinterpret_cast<const int16_t *>(m_ringBuffer + readIndex);
            uint32_t copyFrames = bytesToCopy / frameSize;

            this->writeMixerFrames_( _ioData, renderedFrames, srcSamples, copyFrames );

            readCount += bytesToCopy;
            readable -= bytesToCopy;
            m_playCursorBytes.fetch_add( bytesToCopy, StdAtomic::memory_order_relaxed );

            renderedFrames += copyFrames;
        }

        // SPSC: reader stores readCount with release
        m_readCount.store( readCount, StdAtomic::memory_order_release );

        *_renderedFrames = renderedFrames;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AudioUnitSoundBufferStream::renderMixerFrames( AudioBufferList * _ioData, uint32_t _frameOffset, uint32_t _frames, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition )
    {
        MENGINE_UNUSED( _frameOffset );
        MENGINE_UNUSED( _loop );

        *_outFramePosition = _framePosition;

        UInt32 frames = (UInt32)_frames;
        UInt32 renderedFrames = 0;
        this->renderMixerFrames( _ioData, frames, &renderedFrames );

        return (uint32_t)renderedFrames;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferStream::prebuffer_()
    {
        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 )
        {
            return false;
        }

        for( ;; )
        {
            // SPSC: writer loads readCount with acquire, reads own writeCount relaxed
            uint32_t readCount = m_readCount.load( StdAtomic::memory_order_acquire );
            uint32_t writeCount = m_writeCount.load( StdAtomic::memory_order_relaxed );
            uint32_t readable = writeCount - readCount;
            size_t freeBytes = m_ringBufferSize - readable;

            freeBytes -= freeBytes % frameSize;

            if( freeBytes < frameSize )
            {
                break;
            }

            uint32_t writeIndex = writeCount & m_ringBufferSizeMask;
            size_t capacity = m_ringBufferSize - writeIndex;

            if( capacity > freeBytes )
            {
                capacity = freeBytes;
            }

            capacity -= capacity % frameSize;

            if( capacity == 0 )
            {
                break;
            }

            uint8_t * writeBuffer = m_ringBuffer + writeIndex;
            size_t written = 0;
            if( this->decodeSegment_( writeBuffer, capacity, &written ) == false )
            {
                return false;
            }

            if( written == 0 )
            {
                break;
            }

            // SPSC: writer stores writeCount with release
            uint32_t newWriteCount = writeCount + (uint32_t)written;
            m_writeCount.store( newWriteCount, StdAtomic::memory_order_release );

            if( written < capacity )
            {
                break;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferStream::decodeSegment_( uint8_t * _buffer, size_t _capacity, size_t * const _written )
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
    void AudioUnitSoundBufferStream::resetRingBuffer_()
    {
        m_writeCount.store( 0, StdAtomic::memory_order_relaxed );
        m_readCount.store( 0, StdAtomic::memory_order_relaxed );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AudioUnitSoundBufferStream::getReadableBytes_() const
    {
        uint32_t writeCount = m_writeCount.load( StdAtomic::memory_order_acquire );
        uint32_t readCount = m_readCount.load( StdAtomic::memory_order_relaxed );

        return writeCount - readCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AudioUnitSoundBufferStream::getWritableBytes_() const
    {
        uint32_t readCount = m_readCount.load( StdAtomic::memory_order_acquire );
        uint32_t writeCount = m_writeCount.load( StdAtomic::memory_order_relaxed );

        return (uint32_t)m_ringBufferSize - (writeCount - readCount);
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundBufferStream::writeMixerFrames_( AudioBufferList * _ioData, uint32_t _frameOffset, const int16_t * _src, uint32_t _frames ) const
    {
        if( _ioData == nullptr )
        {
            return;
        }

        if( _src == nullptr )
        {
            return;
        }

        if( m_channels == 0 )
        {
            return;
        }

        if( _frames == 0 )
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

            if( buffer.mData == nullptr )
            {
                return;
            }

            if( buffer.mNumberChannels == 0 )
            {
                return;
            }

            UInt32 destinationChannels = buffer.mNumberChannels;
            Float32 * dst = static_cast<Float32 *>(buffer.mData) + _frameOffset * destinationChannels;

            for( uint32_t frame = 0; frame != _frames; ++frame )
            {
                const int16_t * srcFrame = _src + frame * m_channels;

                for( UInt32 channel = 0; channel != destinationChannels; ++channel )
                {
                    dst[frame * destinationChannels + channel] = Helper::resolveAudioUnitSoundPCM16Sample( srcFrame, m_channels, channel, destinationChannels );
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

            Float32 * dst = static_cast<Float32 *>(buffer.mData) + _frameOffset;

            for( uint32_t frame = 0; frame != _frames; ++frame )
            {
                const int16_t * srcFrame = _src + frame * m_channels;

                dst[frame] = Helper::resolveAudioUnitSoundPCM16Sample( srcFrame, m_channels, channel, bufferCount );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
