#include "WASAPISoundBufferStream.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "WASAPISoundPCMHelper.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"
#include "Config/StdThread.h"

#include <cstring>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundBufferStream::WASAPISoundBufferStream()
        : m_looped( false )
        , m_updating( false )
        , m_finished( false )
        , m_renderBarrier( false )
        , m_readOffset( 0 )
        , m_writeOffset( 0 )
        , m_availableBytes( 0 )
        , m_playCursorBytes( 0 )
        , m_activeRenders( 0 )
        , m_basePositionMs( 0.f )
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
    WASAPISoundBufferStream::~WASAPISoundBufferStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferStream::_acquireSoundBuffer()
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

        size_t totalSize = MENGINE_WASAPI_STREAM_BUFFER_SIZE * MENGINE_WASAPI_STREAM_BUFFER_COUNT;
        memory->newBuffer( totalSize );

        m_memory = memory;
        m_ringBufferSize = totalSize;
        m_ringBuffer = m_memory->getBuffer();

        MemoryBufferInterfacePtr decodeMemory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decodeMemory, "invalid create decode memory" );

        decodeMemory->newBuffer( MENGINE_WASAPI_STREAM_DECODE_BUFFER_SIZE );

        m_memoryDecode = decodeMemory;
        m_decodeBuffer = m_memoryDecode->getBuffer();
        m_decodeBufferSize = MENGINE_WASAPI_STREAM_DECODE_BUFFER_SIZE;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferStream::_releaseSoundBuffer()
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
    bool WASAPISoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();

        m_sourceFrequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;

        if( m_channels == 0 || m_sourceFrequency == 0 )
        {
            LOGGER_ERROR( "invalid stream format frequency: %u channels: %u"
                , m_sourceFrequency
                , m_channels
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
    bool WASAPISoundBufferStream::playSource( bool _looped, float _position )
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
            m_basePositionMs = _position;
            m_playCursorBytes = 0;

            this->resetRingBuffer_();

            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

            this->resetDecodeBuffer_();

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
        }

        m_updating = true;

        this->endMutableState_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferStream::stopSource()
    {
        this->beginMutableState_();

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

            m_updating = false;
            m_finished = true;
            m_basePositionMs = 0.f;
            m_playCursorBytes = 0;

            this->resetRingBuffer_();

            MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );
            this->resetDecodeBuffer_();
        }

        this->endMutableState_();
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferStream::pauseSource()
    {
        m_updating = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferStream::resumeSource()
    {
        m_updating = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferStream::setTimePosition( float _position )
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
        this->resetDecodeBuffer_();

        m_basePositionMs = _position;
        m_playCursorBytes = 0;
        m_finished = false;

        if( this->prebuffer_() == false )
        {
            this->endMutableState_();

            return false;
        }

        this->endMutableState_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferStream::getTimePosition( float * const _position ) const
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
    bool WASAPISoundBufferStream::updateSoundBuffer()
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
    void WASAPISoundBufferStream::setVolume( float _gain )
    {
        MENGINE_UNUSED( _gain );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundBufferStream::renderMixerFrames( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, uint32_t _frames, float _gain, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition )
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
            m_playCursorBytes.fetch_add( bytesToCopy );

            renderedFrames += copyFrames;
        }

        this->leaveRender_();

        return renderedFrames;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferStream::prebuffer_()
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
    bool WASAPISoundBufferStream::decodeSegment_( uint8_t * _buffer, size_t _capacity, size_t * const _written )
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
                dstFrame[channel] = Helper::interpolateWASAPISoundPCM16Sample( frame0, frame1, m_channels, channel, alpha );
            }

            ++totalWrittenFrames;
            m_decodeCursor += m_resampleStep;
        }

        *_written = (size_t)totalWrittenFrames * frameSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferStream::ensureDecodeFrames_( uint32_t _requiredFrames )
    {
        if( m_channels == 0 || m_decodeBuffer == nullptr )
        {
            return false;
        }

        uint32_t frameSize = this->getFrameSize();
        uint32_t baseFrame = (uint32_t)m_decodeCursor;
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
                    if( m_soundDecoder->rewind() == false )
                    {
                        return false;
                    }

                    continue;
                }

                m_decodeEOF = true;
                m_finished = true;

                break;
            }

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
                m_finished = true;

                break;
            }
        }

        uint32_t availableFrames = m_decodeFrames - (uint32_t)m_decodeCursor;

        return availableFrames >= _requiredFrames;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferStream::resetRingBuffer_()
    {
        m_readOffset = 0;
        m_writeOffset = 0;
        m_availableBytes = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferStream::resetDecodeBuffer_()
    {
        m_decodeFrames = 0;
        m_decodeCursor = 0.0;
        m_decodeEOF = false;
        m_resampleStep = m_frequency != 0 ? (double)m_sourceFrequency / (double)m_frequency : 1.0;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferStream::writeMixerFrames_( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, const int16_t * _src, uint32_t _frames, float _gain ) const
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
                dstFrame[channel] += Helper::resolveWASAPISoundPCM16Sample( srcFrame, m_channels, channel, _outputChannels ) * _gain;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferStream::beginMutableState_()
    {
        m_renderBarrier = true;

        while( m_activeRenders.load() != 0 )
        {
            StdThread::yield();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferStream::endMutableState_()
    {
        m_renderBarrier = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferStream::tryEnterRender_()
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
    void WASAPISoundBufferStream::leaveRender_()
    {
        m_activeRenders.fetch_sub( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
}
