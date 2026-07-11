#include "ALSASoundBufferStream.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "ALSASoundPCMHelper.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"
#include "Config/StdThread.h"

#include <cstring>

#ifndef MENGINE_UNIX_STREAM_BUFFER_COUNT
#define MENGINE_UNIX_STREAM_BUFFER_COUNT (4)
#endif

#ifndef MENGINE_UNIX_STREAM_BUFFER_SIZE
#define MENGINE_UNIX_STREAM_BUFFER_SIZE (44100 * 2)
#endif

#ifndef MENGINE_UNIX_STREAM_DECODE_BUFFER_SIZE
#define MENGINE_UNIX_STREAM_DECODE_BUFFER_SIZE (44100 * 2)
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ALSASoundBufferStream::ALSASoundBufferStream()
        : m_looped( false )
        , m_updating( false )
        , m_finished( false )
        , m_renderBarrier( false )
        , m_readOffset( 0 )
        , m_writeOffset( 0 )
        , m_availableBytes( 0 )
        , m_playPositionBytes( 0 )
        , m_activeRenders( 0 )
        , m_ringBufferSize( 0 )
        , m_ringBuffer( nullptr )
        , m_decodeBuffer( nullptr )
        , m_decodeBufferSize( 0 )
        , m_decodeFrames( 0 )
        , m_decodeCursor( 0.0 )
        , m_decodeEOF( false )
        , m_resampleStep( 1.0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ALSASoundBufferStream::~ALSASoundBufferStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::_acquireSoundBuffer()
    {
        ThreadMutexInterfacePtr mutexDecoder = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexDecoder, "invalid create decoder mutex" );

        m_mutexDecoder = mutexDecoder;

        ThreadMutexInterfacePtr mutexUpdating = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexUpdating, "invalid create updating mutex" );

        m_mutexUpdating = mutexUpdating;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create stream memory" );

        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 )
        {
            return false;
        }

        size_t totalSize = MENGINE_UNIX_STREAM_BUFFER_SIZE * MENGINE_UNIX_STREAM_BUFFER_COUNT;
        totalSize -= totalSize % frameSize;
        memory->newBuffer( totalSize );

        m_memory = memory;
        m_ringBufferSize = totalSize;
        m_ringBuffer = m_memory->getBuffer();

        MemoryBufferInterfacePtr decodeMemory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decodeMemory, "invalid create decode memory" );

        size_t decodeBufferSize = MENGINE_UNIX_STREAM_DECODE_BUFFER_SIZE;
        decodeBufferSize -= decodeBufferSize % frameSize;
        decodeMemory->newBuffer( decodeBufferSize );

        m_memoryDecode = decodeMemory;
        m_decodeBuffer = m_memoryDecode->getBuffer();
        m_decodeBufferSize = decodeBufferSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::_releaseSoundBuffer()
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
            m_playPositionBytes = 0;

            m_soundDecoder = nullptr;
            m_memory = nullptr;
            m_ringBuffer = nullptr;
            m_ringBufferSize = 0;

            m_memoryDecode = nullptr;
            m_decodeBuffer = nullptr;
            m_decodeBufferSize = 0;
            m_decodeFrames = 0;
            m_decodeCursor = 0.0;
            m_decodeEOF = false;
            m_resampleStep = 1.0;
        }

        m_mutexDecoder = nullptr;
        m_mutexUpdating = nullptr;

        this->endMutableState_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();

        m_sourceFrequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;

        if( m_channels == 0 || m_channels > 2 || m_sourceFrequency == 0 || dataInfo->bits != 2 )
        {
            LOGGER_ERROR( "invalid PCM16 stream format frequency: %u channels: %u sample bytes: %u"
                , m_sourceFrequency
                , m_channels
                , dataInfo->bits
            );

            return false;
        }

        if( dataInfo->size < (size_t)m_channels * 2 )
        {
            LOGGER_ERROR( "invalid stream data size: %zu"
                , dataInfo->size
            );

            return false;
        }

        m_frequency = m_outputFrequency != 0 ? m_outputFrequency : m_sourceFrequency;
        m_isStereo = m_channels > 1;
        m_resampleStep = (double)m_sourceFrequency / (double)m_frequency;

        this->resetDecodeBuffer_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::playSource( bool _looped, float _position )
    {
        this->beginMutableState_();

        if( _position < 0.f || _position > m_duration )
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

            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

            if( this->seekPosition_( _position ) == false )
            {
                this->endMutableState_();

                return false;
            }
        }

        m_updating = true;

        this->endMutableState_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::stopSource()
    {
        this->beginMutableState_();

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

            m_updating = false;
            m_finished = true;
            m_playPositionBytes = 0;

            this->resetRingBuffer_();

            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );
            this->resetDecodeBuffer_();
        }

        this->endMutableState_();
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::pauseSource()
    {
        m_updating = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::resumeSource()
    {
        m_updating = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::setTimePosition( float _position )
    {
        if( _position < 0.f || _position > m_duration )
        {
            return false;
        }

        this->beginMutableState_();

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

            if( this->seekPosition_( _position ) == false )
            {
                this->endMutableState_();

                return false;
            }
        }

        this->endMutableState_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::setLoopSource( bool _looped )
    {
        if( m_mutexUpdating == nullptr || m_mutexDecoder == nullptr )
        {
            m_looped = _looped;

            return true;
        }

        this->beginMutableState_();

        bool successful = true;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

            bool previousLooped = m_looped.exchange( _looped );

            if( previousLooped == true && _looped == false && m_duration > 0.f )
            {
                uint32_t bytesPerSecond = m_frequency * m_channels * 2;
                uint64_t playPositionBytes = m_playPositionBytes.load();
                float position = (float)((double)playPositionBytes * 1000.0 / (double)bytesPerSecond);
                position = StdMath::fmodf( position, m_duration );

                if( this->seekPosition_( position ) == false )
                {
                    m_looped = previousLooped;
                    successful = false;

                    LOGGER_ERROR( "failed to disable stream loop at position %f"
                        , position
                    );
                }
            }
            else if( previousLooped == false && _looped == true && m_decodeEOF == true )
            {
                bool previousFinished = m_finished.load();

                if( m_soundDecoder->rewind() == false )
                {
                    m_looped = previousLooped;
                    successful = false;

                    LOGGER_ERROR( "failed to rewind stream after enabling loop" );
                }
                else
                {
                    m_decodeEOF = false;
                    m_finished = false;

                    if( this->prebuffer_() == false )
                    {
                        m_looped = previousLooped;
                        m_decodeEOF = true;
                        m_finished = previousFinished;
                        successful = false;

                        LOGGER_ERROR( "failed to prebuffer stream after enabling loop" );
                    }
                }
            }
        }

        this->endMutableState_();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::getTimePosition( float * const _position ) const
    {
        uint32_t bytesPerSecond = m_frequency * m_channels * 2;

        if( bytesPerSecond == 0 )
        {
            *_position = 0.f;

            return true;
        }

        uint64_t absBytes = m_playPositionBytes.load();
        float position = (float)((double)absBytes * 1000.0 / (double)bytesPerSecond);

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
    bool ALSASoundBufferStream::updateSoundBuffer()
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

        // Keep the worker alive until SoundService stops the source. This lets a
        // non-looping stream be switched back to looping after reaching EOF.
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::isEndOfStream() const
    {
        if( m_finished.load() == true && m_availableBytes.load() == 0 )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::setVolume( float _gain )
    {
        MENGINE_UNUSED( _gain );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ALSASoundBufferStream::renderMixerFrames( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, uint32_t _frames, float _gain, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition )
    {
        MENGINE_UNUSED( _framePosition );
        MENGINE_UNUSED( _loop );

        *_outFramePosition = _framePosition;

        if( _mixBuffer == nullptr || _frames == 0 || _outputChannels == 0 )
        {
            return 0;
        }

        if( this->tryEnterRender_() == false )
        {
            return 0;
        }

        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 || m_ringBuffer == nullptr )
        {
            this->leaveRender_();

            return 0;
        }

        uint32_t renderedFrames = 0;

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

            this->writeMixerFrames_( _mixBuffer, _outputChannels, _frameOffset + renderedFrames, srcSamples, copyFrames, _gain );

            m_readOffset.store( (readOffset + bytesToCopy) % (uint32_t)m_ringBufferSize );
            m_availableBytes.fetch_sub( bytesToCopy );
            m_playPositionBytes.fetch_add( bytesToCopy );

            renderedFrames += copyFrames;
        }

        this->leaveRender_();

        return renderedFrames;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::prebuffer_()
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

        bool decodeDrained = m_decodeEOF == true && (uint32_t)m_decodeCursor >= m_decodeFrames;
        m_finished = decodeDrained;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::seekPosition_( float _position )
    {
        float position = _position;

        if( m_looped.load() == true && m_duration > 0.f && position >= m_duration )
        {
            position = 0.f;
        }

        uint32_t bytesPerSecond = m_frequency * m_channels * 2;

        if( m_looped.load() == false && m_duration > 0.f && position >= m_duration )
        {
            this->resetRingBuffer_();
            this->resetDecodeBuffer_();

            m_decodeEOF = true;
            m_finished = true;
            m_playPositionBytes.store( (uint64_t)((double)m_duration / 1000.0 * (double)bytesPerSecond) );

            return true;
        }

        if( m_soundDecoder->seek( position ) == false )
        {
            LOGGER_ASSERTION( "invalid seek '%f'"
                , position
            );

            return false;
        }

        this->resetRingBuffer_();
        this->resetDecodeBuffer_();
        m_finished = false;

        if( this->prebuffer_() == false )
        {
            return false;
        }

        m_playPositionBytes.store( (uint64_t)((double)position / 1000.0 * (double)bytesPerSecond) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::decodeSegment_( uint8_t * _buffer, size_t _capacity, size_t * const _written )
    {
        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 || _buffer == nullptr )
        {
            *_written = 0;

            return false;
        }

        uint32_t capacityFrames = (uint32_t)(_capacity / frameSize);
        uint32_t totalWrittenFrames = 0;
        int16_t * dstFrames = reinterpret_cast<int16_t *>(_buffer);

        while( totalWrittenFrames != capacityFrames )
        {
            bool hasLinearPair = this->ensureDecodeFrames_( 2 );
            uint32_t baseFrame = (uint32_t)m_decodeCursor;

            if( baseFrame >= m_decodeFrames )
            {
                break;
            }

            float alpha = (float)(m_decodeCursor - (double)baseFrame);
            uint32_t availableFrames = m_decodeFrames - baseFrame;

            if( availableFrames == 0 )
            {
                break;
            }

            const int16_t * frame0 = reinterpret_cast<const int16_t *>(m_decodeBuffer) + baseFrame * m_channels;
            const int16_t * frame1 = frame0;

            if( hasLinearPair == true && availableFrames > 1 )
            {
                frame1 = frame0 + m_channels;
            }

            int16_t * dstFrame = dstFrames + totalWrittenFrames * m_channels;

            for( uint32_t channel = 0; channel != m_channels; ++channel )
            {
                dstFrame[channel] = Helper::interpolateALSASoundPCM16Sample( frame0, frame1, m_channels, channel, alpha );
            }

            ++totalWrittenFrames;
            m_decodeCursor += m_resampleStep;
        }

        *_written = (size_t)totalWrittenFrames * frameSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::ensureDecodeFrames_( uint32_t _requiredFrames )
    {
        if( m_channels == 0 || m_decodeBuffer == nullptr )
        {
            return false;
        }

        uint32_t frameSize = this->getFrameSize();
        uint32_t baseFrame = (uint32_t)m_decodeCursor;
        uint32_t availableFrames = m_decodeFrames > baseFrame ? m_decodeFrames - baseFrame : 0;

        if( availableFrames >= _requiredFrames )
        {
            return true;
        }

        double fraction = m_decodeCursor - (double)baseFrame;

        if( baseFrame != 0 )
        {
            uint32_t remainingFrames = m_decodeFrames > baseFrame ? m_decodeFrames - baseFrame : 0;

            if( remainingFrames != 0 )
            {
                std::memmove( m_decodeBuffer, m_decodeBuffer + baseFrame * frameSize, (size_t)remainingFrames * frameSize );
            }

            m_decodeFrames = remainingFrames;
            m_decodeCursor = fraction;
        }

        bool rewindAfterEmpty = false;

        while( m_decodeEOF == false && (m_decodeFrames - (uint32_t)m_decodeCursor) < _requiredFrames )
        {
            size_t usedBytes = (size_t)m_decodeFrames * frameSize;
            size_t freeBytes = m_decodeBufferSize - usedBytes;

            if( freeBytes < frameSize )
            {
                break;
            }

            SoundDecoderData decoderData;
            decoderData.buffer = m_decodeBuffer + usedBytes;
            decoderData.size = freeBytes;

            size_t bytesWritten = m_soundDecoder->decode( &decoderData );
            bytesWritten -= bytesWritten % frameSize;

            if( bytesWritten == 0 )
            {
                if( m_looped.load() == true )
                {
                    if( rewindAfterEmpty == true )
                    {
                        m_decodeEOF = true;

                        break;
                    }

                    if( m_soundDecoder->rewind() == false )
                    {
                        return false;
                    }

                    rewindAfterEmpty = true;

                    continue;
                }

                m_decodeEOF = true;

                break;
            }

            rewindAfterEmpty = false;
            m_decodeFrames += (uint32_t)(bytesWritten / frameSize);

            if( bytesWritten < decoderData.size )
            {
                if( m_looped.load() == true )
                {
                    if( m_soundDecoder->rewind() == false )
                    {
                        return false;
                    }

                    continue;
                }

                m_decodeEOF = true;

                break;
            }
        }

        availableFrames = m_decodeFrames - (uint32_t)m_decodeCursor;

        if( availableFrames >= _requiredFrames )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::resetRingBuffer_()
    {
        m_readOffset = 0;
        m_writeOffset = 0;
        m_availableBytes = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::resetDecodeBuffer_()
    {
        m_decodeFrames = 0;
        m_decodeCursor = 0.0;
        m_decodeEOF = false;
        m_resampleStep = m_frequency != 0 ? (double)m_sourceFrequency / (double)m_frequency : 1.0;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::writeMixerFrames_( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, const int16_t * _src, uint32_t _frames, float _gain ) const
    {
        if( _mixBuffer == nullptr || _src == nullptr || m_channels == 0 || _frames == 0 || _outputChannels == 0 )
        {
            return;
        }

        for( uint32_t frame = 0; frame != _frames; ++frame )
        {
            const int16_t * srcFrame = _src + frame * m_channels;
            float * dstFrame = _mixBuffer + (_frameOffset + frame) * _outputChannels;

            for( uint32_t channel = 0; channel != _outputChannels; ++channel )
            {
                dstFrame[channel] += Helper::resolveALSASoundPCM16Sample( srcFrame, m_channels, channel, _outputChannels ) * _gain;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::beginMutableState_()
    {
        m_renderBarrier = true;

        while( m_activeRenders.load() != 0 )
        {
            StdThread::yield();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferStream::endMutableState_()
    {
        m_renderBarrier = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferStream::tryEnterRender_()
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
    void ALSASoundBufferStream::leaveRender_()
    {
        m_activeRenders.fetch_sub( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
}
