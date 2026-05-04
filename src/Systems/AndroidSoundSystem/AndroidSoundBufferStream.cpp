#include "AndroidSoundBufferStream.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundBufferStream::AndroidSoundBufferStream()
        : m_currentBuffer( 0 )
        , m_looped( false )
        , m_updating( false )
        , m_consumedCount( 0 )
        , m_decoderEOS( false )
    {
        StdAlgorithm::fill_n( m_bufferData, (size_t)MENGINE_ANDROID_STREAM_BUFFER_COUNT, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundBufferStream::~AndroidSoundBufferStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferStream::_acquireSoundBuffer()
    {
        ThreadMutexInterfacePtr mutexUpdating = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexUpdating, "invalid create mutex" );

        m_mutexUpdating = mutexUpdating;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        size_t totalSize = MENGINE_ANDROID_STREAM_BUFFER_SIZE * MENGINE_ANDROID_STREAM_BUFFER_COUNT;
        memory->newBuffer( totalSize );

        m_memory = memory;

        uint8_t * baseBuffer = m_memory->getBuffer();

        for( uint32_t i = 0; i != MENGINE_ANDROID_STREAM_BUFFER_COUNT; ++i )
        {
            m_bufferData[i] = baseBuffer + i * MENGINE_ANDROID_STREAM_BUFFER_SIZE;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferStream::_releaseSoundBuffer()
    {
        this->setUpdating_( false );

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

            this->destroyPlayer_();

            for( uint32_t i = 0; i != MENGINE_ANDROID_STREAM_BUFFER_COUNT; ++i )
            {
                m_bufferData[i] = nullptr;
            }

            m_soundDecoder = nullptr;
            m_memory = nullptr;
        }

        m_mutexUpdating = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
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
    void AndroidSoundBufferStream::bufferQueueCallback_( SLAndroidSimpleBufferQueueItf _bq, void * _context )
    {
        MENGINE_UNUSED( _bq );

        AndroidSoundBufferStream * stream = static_cast<AndroidSoundBufferStream *>(_context);
        stream->onBufferConsumed_();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferStream::onBufferConsumed_()
    {
        ++m_consumedCount;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferStream::playSource( bool _looped, float _position )
    {
        if( m_playerObject != nullptr )
        {
            this->destroyPlayer_();
        }

        m_looped = _looped;
        m_consumedCount = 0;
        m_currentBuffer = 0;
        m_finished = false;
        m_decoderEOS = false;

        if( this->createPlayer_() == false )
        {
            return false;
        }

        (*m_bufferQueueItf)->RegisterCallback( m_bufferQueueItf, &AndroidSoundBufferStream::bufferQueueCallback_, this );

        if( _position > m_duration )
        {
            LOGGER_ASSERTION( "pos %f > length %f"
                , _position
                , m_duration
            );

            this->destroyPlayer_();

            return false;
        }

        if( m_soundDecoder->seek( _position ) == false )
        {
            LOGGER_ASSERTION( "invalid seek '%f'"
                , _position
            );

            this->destroyPlayer_();

            return false;
        }

        for( uint32_t i = 0; i != MENGINE_ANDROID_STREAM_BUFFER_COUNT; ++i )
        {
            size_t bytesWritten;
            if( this->fillBuffer_( i, &bytesWritten ) == false )
            {
                this->destroyPlayer_();

                return false;
            }

            if( bytesWritten == 0 )
            {
                break;
            }

            SLresult result = (*m_bufferQueueItf)->Enqueue( m_bufferQueueItf, m_bufferData[i], (SLuint32)bytesWritten );

            if( result != SL_RESULT_SUCCESS )
            {
                LOGGER_ASSERTION( "failed to enqueue buffer %u [%d]"
                    , i
                    , result
                );

                this->destroyPlayer_();

                return false;
            }

            if( bytesWritten != MENGINE_ANDROID_STREAM_BUFFER_SIZE )
            {
                break;
            }
        }

        m_currentBuffer = 0;

        SLresult result = (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_PLAYING );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to set play state [%d]"
                , result
            );

            this->destroyPlayer_();

            return false;
        }

        this->setUpdating_( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferStream::stopSource()
    {
        this->setUpdating_( false );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

        if( m_playerObject == nullptr )
        {
            return;
        }

        (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_STOPPED );
        (*m_bufferQueueItf)->Clear( m_bufferQueueItf );

        this->destroyPlayer_();

        m_finished = false;
        m_decoderEOS = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferStream::pauseSource()
    {
        if( m_playerObject == nullptr )
        {
            return;
        }

        (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_PAUSED );

        this->setUpdating_( false );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferStream::resumeSource()
    {
        if( m_playerObject == nullptr )
        {
            return;
        }

        this->setUpdating_( true );

        (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_PLAYING );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferStream::setUpdating_( bool _updating )
    {
        if( m_updating == _updating )
        {
            return;
        }

        m_mutexUpdating->lock();
        m_updating = _updating;
        m_mutexUpdating->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferStream::setTimePosition( float _position )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

        bool updating = m_updating == true;

        if( m_playerObject == nullptr )
        {
            return false;
        }

        (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_STOPPED );
        (*m_bufferQueueItf)->Clear( m_bufferQueueItf );

        if( m_soundDecoder->seek( _position ) == false )
        {
            return false;
        }

        m_consumedCount = 0;
        m_currentBuffer = 0;
        m_finished = false;
        m_decoderEOS = false;

        for( uint32_t i = 0; i != MENGINE_ANDROID_STREAM_BUFFER_COUNT; ++i )
        {
            size_t bytesWritten;
            if( this->fillBuffer_( i, &bytesWritten ) == false )
            {
                return false;
            }

            if( bytesWritten == 0 )
            {
                break;
            }

            SLresult result = (*m_bufferQueueItf)->Enqueue( m_bufferQueueItf, m_bufferData[i], (SLuint32)bytesWritten );

            if( result != SL_RESULT_SUCCESS )
            {
                return false;
            }

            if( bytesWritten != MENGINE_ANDROID_STREAM_BUFFER_SIZE )
            {
                break;
            }
        }

        m_currentBuffer = 0;

        SLresult result = (*m_playItf)->SetPlayState( m_playItf, updating == true ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED );

        if( result != SL_RESULT_SUCCESS )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferStream::getTimePosition( float * const _position ) const
    {
        float timeTell = m_soundDecoder->tell();

        if( timeTell > m_duration )
        {
            LOGGER_ASSERTION( "get tell %f > length %f"
                , timeTell
                , m_duration
            );

            return false;
        }

        *_position = timeTell;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferStream::updateSoundBuffer()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

        if( m_updating == false )
        {
            return true;
        }

        if( m_playerObject == nullptr )
        {
            return true;
        }

        uint32_t consumed = m_consumedCount.exchange( 0 );

        for( uint32_t i = 0; i != consumed; ++i )
        {
            uint32_t bufIdx = m_currentBuffer;
            m_currentBuffer = (m_currentBuffer + 1) % MENGINE_ANDROID_STREAM_BUFFER_COUNT;

            size_t bytesWritten = 0;
            if( this->fillBuffer_( bufIdx, &bytesWritten ) == false )
            {
                continue;
            }

            if( bytesWritten == 0 )
            {
                continue;
            }

            (*m_bufferQueueItf)->Enqueue( m_bufferQueueItf, m_bufferData[bufIdx], (SLuint32)bytesWritten );
        }

        // Detect natural end-of-stream: OpenSL ES does not transition the play state to STOPPED
        // when the buffer queue underruns, so we have to check it ourselves.
        if( m_looped == false && m_decoderEOS == true )
        {
            SLAndroidSimpleBufferQueueState queueState;
            (*m_bufferQueueItf)->GetState( m_bufferQueueItf, &queueState );

            if( queueState.count == 0 )
            {
                (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_STOPPED );

                m_finished = true;

                return false;
            }
        }

        SLuint32 playState;
        (*m_playItf)->GetPlayState( m_playItf, &playState );

        switch( playState )
        {
        case SL_PLAYSTATE_PLAYING:
            {
                //Empty
            }break;
        case SL_PLAYSTATE_PAUSED:
            {
                //Empty
            }break;
        case SL_PLAYSTATE_STOPPED:
            {
                if( m_looped == false )
                {
                    SLAndroidSimpleBufferQueueState state;
                    (*m_bufferQueueItf)->GetState( m_bufferQueueItf, &state );

                    if( state.count == 0 )
                    {
                        m_finished = true;

                        return false;
                    }
                }

                (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_PLAYING );
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferStream::fillBuffer_( uint32_t _bufferIndex, size_t * const _bytes )
    {
        uint8_t * dataBuffer = m_bufferData[_bufferIndex];

        SoundDecoderData decoderData;
        decoderData.buffer = dataBuffer;
        decoderData.size = MENGINE_ANDROID_STREAM_BUFFER_SIZE;

        size_t bytesWritten = m_soundDecoder->decode( &decoderData );

        if( m_looped == true && bytesWritten != MENGINE_ANDROID_STREAM_BUFFER_SIZE )
        {
            if( m_soundDecoder->rewind() == false )
            {
                return false;
            }

            size_t bufferSize = MENGINE_ANDROID_STREAM_BUFFER_SIZE - bytesWritten;

            bufferSize -= bufferSize % 4;

            SoundDecoderData decoderDataTail;
            decoderDataTail.buffer = dataBuffer + bytesWritten;
            decoderDataTail.size = bufferSize;

            size_t bytesWritten2 = m_soundDecoder->decode( &decoderDataTail );

            bytesWritten += bytesWritten2;
        }
        else if( m_looped == false && bytesWritten != MENGINE_ANDROID_STREAM_BUFFER_SIZE )
        {
            // Decoder reached the end of the stream and we are not looping; remember it so
            // updateSoundBuffer() can finish playback once the queued buffers drain.
            m_decoderEOS = true;
        }

        *_bytes = bytesWritten;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferStream::setVolume( float _gain )
    {
        if( m_volumeItf == nullptr )
        {
            return;
        }

        SLmillibel mb = AndroidSoundBufferBase::gainToMillibels_( _gain );

        (*m_volumeItf)->SetVolumeLevel( m_volumeItf, mb );
    }
    //////////////////////////////////////////////////////////////////////////
}
