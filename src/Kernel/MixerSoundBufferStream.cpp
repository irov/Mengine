#include "MixerSoundBufferStream.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/MixerSoundPCMHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/SpinLockScope.h"
#include "Kernel/SpinLockTryScope.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"
#include "Config/StdMemory.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MIXER_STREAM_BUFFER_COUNT
#define MENGINE_MIXER_STREAM_BUFFER_COUNT (4)
#endif

#ifndef MENGINE_MIXER_STREAM_BUFFER_SIZE
#define MENGINE_MIXER_STREAM_BUFFER_SIZE (44100 * 2)
#endif

#ifndef MENGINE_MIXER_STREAM_DECODE_BUFFER_SIZE
#define MENGINE_MIXER_STREAM_DECODE_BUFFER_SIZE (44100 * 2)
#endif
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MixerSoundBufferStream::MixerSoundBufferStream()
        : m_looped( false )
        , m_updating( false )
        , m_finished( false )
        , m_readOffset( 0 )
        , m_writeOffset( 0 )
        , m_availableBytes( 0 )
        , m_playPositionBytes( 0 )
        , m_ringBufferSize( 0 )
        , m_ringBuffer( nullptr )
        , m_decodeBuffer( nullptr )
        , m_decodeBufferSize( 0 )
        , m_decodeFrames( 0 )
        , m_decodePhase( 0 )
        , m_decodeEOF( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MixerSoundBufferStream::~MixerSoundBufferStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::_acquireSoundBuffer()
    {
        ThreadMutexInterfacePtr mutexDecoder = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexDecoder, "invalid create decoder mutex" );

        m_mutexDecoder = mutexDecoder;

        ThreadMutexInterfacePtr mutexUpdating = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexUpdating, "invalid create updating mutex" );

        m_mutexUpdating = mutexUpdating;

        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 )
        {
            return false;
        }

        size_t totalSize = MENGINE_MIXER_STREAM_BUFFER_SIZE * MENGINE_MIXER_STREAM_BUFFER_COUNT;
        totalSize -= totalSize % frameSize;
        MemoryInterfacePtr memory = Helper::createMemoryBuffer( totalSize, MENGINE_DOCUMENT_FACTORABLE );

        m_memory = memory;
        m_ringBufferSize = totalSize;
        m_ringBuffer = m_memory->getBuffer();

        size_t decodeBufferSize = MENGINE_MIXER_STREAM_DECODE_BUFFER_SIZE;
        decodeBufferSize -= decodeBufferSize % frameSize;
        MemoryInterfacePtr decodeMemory = Helper::createMemoryBuffer( decodeBufferSize, MENGINE_DOCUMENT_FACTORABLE );

        m_memoryDecode = decodeMemory;
        m_decodeBuffer = m_memoryDecode->getBuffer();
        m_decodeBufferSize = decodeBufferSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::_releaseSoundBuffer()
    {
        MENGINE_SPINLOCK_SCOPE( m_renderLock );

        this->releaseStream_();

        m_mutexDecoder = nullptr;
        m_mutexUpdating = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();

        m_sourceFrequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;

        if( m_channels == 0 )
        {
            LOGGER_ERROR( "invalid PCM16 stream channels: %u"
                , m_channels
            );

            return false;
        }

        if( m_channels > 2 )
        {
            LOGGER_ERROR( "invalid PCM16 stream channels: %u"
                , m_channels
            );

            return false;
        }

        if( m_sourceFrequency == 0 )
        {
            LOGGER_ERROR( "invalid PCM16 stream frequency: %u"
                , m_sourceFrequency
            );

            return false;
        }

        if( dataInfo->bits != sizeof( int16_t ) )
        {
            LOGGER_ERROR( "invalid PCM16 stream sample bytes: %u"
                , dataInfo->bits
            );

            return false;
        }

        if( dataInfo->size < (size_t)m_channels * sizeof( int16_t ) )
        {
            LOGGER_ERROR( "invalid stream data size: %zu"
                , dataInfo->size
            );

            return false;
        }

        m_frequency = m_outputFrequency != 0 ? m_outputFrequency : m_sourceFrequency;
        m_isStereo = m_channels > 1;

        this->resetDecodeBuffer_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::playSource( bool _looped, float _position )
    {
        MENGINE_ASSERTION( StdMath::isfinite( _position ) == true, "invalid sound position %f"
            , _position
        );

        MENGINE_SPINLOCK_SCOPE( m_renderLock );

        if( _position < 0.f )
        {
            LOGGER_ASSERTION( "pos %f > length %f"
                , _position
                , m_duration
            );

            return false;
        }

        if( _position > m_duration )
        {
            LOGGER_ASSERTION( "pos %f > length %f"
                , _position
                , m_duration
            );

            return false;
        }

        if( this->prepareSource_( _looped, _position ) == false )
        {
            return false;
        }

        m_updating = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::stopSource()
    {
        MENGINE_SPINLOCK_SCOPE( m_renderLock );

        this->stopSource_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::pauseSource()
    {
        m_updating = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::resumeSource()
    {
        m_updating = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::setTimePosition( float _position )
    {
        MENGINE_ASSERTION( StdMath::isfinite( _position ) == true, "invalid sound position %f"
            , _position
        );

        if( _position < 0.f )
        {
            return false;
        }

        if( _position > m_duration )
        {
            return false;
        }

        MENGINE_SPINLOCK_SCOPE( m_renderLock );

        if( this->seekSource_( _position ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::setLoopSource( bool _looped )
    {
        if( m_mutexUpdating == nullptr )
        {
            m_looped = _looped;

            return true;
        }

        if( m_mutexDecoder == nullptr )
        {
            m_looped = _looped;

            return true;
        }

        MENGINE_SPINLOCK_SCOPE( m_renderLock );

        if( this->setLoopSource_( _looped ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::getTimePosition( float * const _position ) const
    {
        uint32_t bytesPerSecond = m_frequency * m_channels * sizeof( int16_t );

        if( bytesPerSecond == 0 )
        {
            *_position = 0.f;

            return true;
        }

        uint64_t absBytes = m_playPositionBytes.load();
        float position = (float)((double)absBytes * 1000.0 / (double)bytesPerSecond);

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
    bool MixerSoundBufferStream::updateSoundBuffer()
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::isEndOfStream() const
    {
        if( m_finished.load() == false )
        {
            return false;
        }

        if( m_availableBytes.load() != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::setVolume( float _gain )
    {
        MENGINE_UNUSED( _gain );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundBufferStream::renderMixerFrames( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, uint32_t _frames, float _gain, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition )
    {
        MENGINE_UNUSED( _framePosition );
        MENGINE_UNUSED( _loop );

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

        SpinLockTryScope renderScope( m_renderLock );

        if( renderScope.isLocked() == false )
        {
            return 0;
        }

        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 )
        {
            return 0;
        }

        if( m_ringBuffer == nullptr )
        {
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

            uint32_t frameOffset = _frameOffset + renderedFrames;
            this->writeMixerFrames_( _mixBuffer, _outputChannels, frameOffset, srcSamples, copyFrames, _gain );

            uint32_t nextReadOffset = (readOffset + bytesToCopy) % (uint32_t)m_ringBufferSize;
            m_readOffset.store( nextReadOffset );
            m_availableBytes.fetch_sub( bytesToCopy );
            m_playPositionBytes.fetch_add( bytesToCopy );

            renderedFrames += copyFrames;
        }

        return renderedFrames;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::releaseStream_()
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
        m_decodePhase = 0;
        m_decodeEOF = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::prepareSource_( bool _looped, float _position )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

        m_looped = _looped;
        m_updating = false;

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

        if( this->seekPosition_( _position ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::stopSource_()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

        m_updating = false;
        m_finished = true;
        m_playPositionBytes = 0;

        this->resetRingBuffer_();

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );
        this->resetDecodeBuffer_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::seekSource_( float _position )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

        if( this->seekPosition_( _position ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::setLoopSource_( bool _looped )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexDecoder );

        bool previousLooped = m_looped.exchange( _looped );

        if( previousLooped == _looped )
        {
            return true;
        }

        if( _looped == false )
        {
            if( m_duration > 0.f )
            {
                uint32_t bytesPerSecond = m_frequency * m_channels * sizeof( int16_t );
                uint64_t playPositionBytes = m_playPositionBytes.load();
                float position = (float)((double)playPositionBytes * 1000.0 / (double)bytesPerSecond);
                position = StdMath::fmodf( position, m_duration );

                if( this->seekPosition_( position ) == false )
                {
                    m_looped = previousLooped;

                    LOGGER_ERROR( "failed to disable stream loop at position %f"
                        , position
                    );

                    return false;
                }
            }

            return true;
        }

        if( m_decodeEOF == false )
        {
            return true;
        }

        bool previousFinished = m_finished.load();

        if( m_soundDecoder->rewind() == false )
        {
            m_looped = previousLooped;

            LOGGER_ERROR( "failed to rewind stream after enabling loop" );

            return false;
        }

        m_decodeEOF = false;
        m_finished = false;

        if( this->prebuffer_() == false )
        {
            m_looped = previousLooped;
            m_decodeEOF = true;
            m_finished = previousFinished;

            LOGGER_ERROR( "failed to prebuffer stream after enabling loop" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::prebuffer_()
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

            uint8_t * writeBuffer = m_ringBuffer + writeOffset;
            size_t written;
            if( this->decodeSegment_( writeBuffer, capacity, &written ) == false )
            {
                return false;
            }

            if( written == 0 )
            {
                break;
            }

            uint32_t nextWriteOffset = (writeOffset + (uint32_t)written) % (uint32_t)m_ringBufferSize;
            m_writeOffset.store( nextWriteOffset );
            m_availableBytes.fetch_add( (uint32_t)written );

            if( written < capacity )
            {
                break;
            }
        }

        if( m_decodeEOF == false )
        {
            m_finished = false;

            return true;
        }

        if( (uint32_t)(m_decodePhase / m_frequency) < m_decodeFrames )
        {
            m_finished = false;

            return true;
        }

        m_finished = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::seekPosition_( float _position )
    {
        float position = _position;

        if( m_looped.load() == true )
        {
            if( m_duration > 0.f )
            {
                if( position >= m_duration )
                {
                    position = 0.f;
                }
            }
        }

        uint32_t bytesPerSecond = m_frequency * m_channels * sizeof( int16_t );

        if( m_looped.load() == false )
        {
            if( m_duration > 0.f )
            {
                if( position >= m_duration )
                {
                    this->resetRingBuffer_();
                    this->resetDecodeBuffer_();

                    m_decodeEOF = true;
                    m_finished = true;
                    uint64_t playPositionBytes = (uint64_t)((double)m_duration / 1000.0 * (double)bytesPerSecond);
                    m_playPositionBytes = playPositionBytes;

                    return true;
                }
            }
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

        uint64_t playPositionBytes = (uint64_t)((double)position / 1000.0 * (double)bytesPerSecond);
        m_playPositionBytes = playPositionBytes;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::decodeSegment_( uint8_t * _buffer, size_t _capacity, size_t * const _written )
    {
        uint32_t frameSize = this->getFrameSize();

        if( frameSize == 0 )
        {
            *_written = 0;

            return false;
        }

        if( _buffer == nullptr )
        {
            *_written = 0;

            return false;
        }

        uint32_t capacityFrames = (uint32_t)(_capacity / frameSize);
        uint32_t totalWrittenFrames = 0;
        int16_t * dstFrames = reinterpret_cast<int16_t *>(_buffer);

        while( totalWrittenFrames != capacityFrames )
        {
            uint32_t availableFrames;
            if( this->ensureDecodeFrames_( 2, &availableFrames ) == false )
            {
                *_written = 0;

                return false;
            }

            if( availableFrames == 0 )
            {
                break;
            }

            uint32_t baseFrame = (uint32_t)(m_decodePhase / m_frequency);
            float alpha = (float)(m_decodePhase % m_frequency) / (float)m_frequency;

            const int16_t * frame0 = reinterpret_cast<const int16_t *>(m_decodeBuffer) + baseFrame * m_channels;
            const int16_t * frame1 = frame0;

            if( availableFrames > 1 )
            {
                frame1 = frame0 + m_channels;
            }

            int16_t * dstFrame = dstFrames + totalWrittenFrames * m_channels;

            for( uint32_t channel = 0; channel != m_channels; ++channel )
            {
                dstFrame[channel] = Helper::interpolateMixerSoundPCM16Sample( frame0, frame1, m_channels, channel, alpha );
            }

            ++totalWrittenFrames;
            m_decodePhase += m_sourceFrequency;
        }

        *_written = (size_t)totalWrittenFrames * frameSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferStream::ensureDecodeFrames_( uint32_t _requiredFrames, uint32_t * const _availableFrames )
    {
        *_availableFrames = 0;

        if( m_channels == 0 )
        {
            return false;
        }

        if( m_decodeBuffer == nullptr )
        {
            return false;
        }

        uint32_t frameSize = this->getFrameSize();
        uint32_t baseFrame = (uint32_t)(m_decodePhase / m_frequency);
        uint32_t availableFrames = m_decodeFrames > baseFrame ? m_decodeFrames - baseFrame : 0;

        if( availableFrames >= _requiredFrames )
        {
            *_availableFrames = availableFrames;

            return true;
        }

        if( baseFrame != 0 )
        {
            uint32_t consumedFrames = StdAlgorithm::min( baseFrame, m_decodeFrames );
            uint32_t remainingFrames = m_decodeFrames - consumedFrames;

            if( remainingFrames != 0 )
            {
                uint8_t * remainingBuffer = m_decodeBuffer + consumedFrames * frameSize;
                size_t remainingSize = (size_t)remainingFrames * frameSize;
                MENGINE_MEMMOVE( m_decodeBuffer, remainingBuffer, remainingSize );
            }

            m_decodeFrames = remainingFrames;
            m_decodePhase -= (uint64_t)consumedFrames * m_frequency;
            baseFrame = (uint32_t)(m_decodePhase / m_frequency);
        }

        bool rewindAfterEmpty = false;

        for( ;; )
        {
            if( m_decodeEOF == true )
            {
                break;
            }

            if( m_decodeFrames > baseFrame )
            {
                if( m_decodeFrames - baseFrame >= _requiredFrames )
                {
                    break;
                }
            }

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
                        LOGGER_ERROR( "looped stream decoder is empty after rewind" );

                        return false;
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

        *_availableFrames = m_decodeFrames > baseFrame ? m_decodeFrames - baseFrame : 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::resetRingBuffer_()
    {
        m_readOffset = 0;
        m_writeOffset = 0;
        m_availableBytes = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::resetDecodeBuffer_()
    {
        m_decodeFrames = 0;
        m_decodePhase = 0;
        m_decodeEOF = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferStream::writeMixerFrames_( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, const int16_t * _src, uint32_t _frames, float _gain ) const
    {
        if( _mixBuffer == nullptr )
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

        if( _outputChannels == 0 )
        {
            return;
        }

        for( uint32_t frame = 0; frame != _frames; ++frame )
        {
            const int16_t * srcFrame = _src + frame * m_channels;
            float * dstFrame = _mixBuffer + (_frameOffset + frame) * _outputChannels;

            for( uint32_t channel = 0; channel != _outputChannels; ++channel )
            {
                float sample = Helper::resolveMixerSoundPCM16Sample( srcFrame, m_channels, channel, _outputChannels );
                dstFrame[channel] += sample * _gain;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
