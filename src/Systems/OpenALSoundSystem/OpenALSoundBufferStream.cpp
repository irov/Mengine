#include "OpenALSoundBufferStream.h"
#include "OpenALSoundSystem.h"
#include "OpenALSoundError.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferStream::OpenALSoundBufferStream()
        : m_sourceId( 0 )
        , m_looped( false )
        , m_updating( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferStream::~OpenALSoundBufferStream()
    {        
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::_acquire()
    {
        ThreadMutexInterfacePtr mutexUpdating = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexUpdating );

        m_mutexUpdating = mutexUpdating;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        memory->newBuffer( MENGINE_OPENAL_STREAM_BUFFER_SIZE );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::_release()
    {
        this->removeBuffers_();

        m_soundDecoder = nullptr;
        m_mutexUpdating = nullptr;
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::removeBuffers_()
    {
        this->setUpdating_( false );

        for( uint32_t i = 0; i != MENGINE_OPENAL_STREAM_BUFFER_COUNT; ++i )
        {
            ALuint id = m_alBuffersId[i];

            if( id == 0 )
            {
                continue;
            }

            m_soundSystem->releaseBufferId( id );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        for( uint32_t i = 0; i != MENGINE_OPENAL_STREAM_BUFFER_COUNT; ++i )
        {
            ALuint id = m_soundSystem->genBufferId();

            if( id == 0 )
            {
                // TODO: report in case of error
                LOGGER_ERROR( "invalid gen %u buffer ID"
                    , i
                );

                this->removeBuffers_();

                return false;
            }

            m_alBuffersId[i] = id;
        }

        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo * dataInfo = m_soundDecoder->getCodecDataInfo();
        m_frequency = dataInfo->frequency;

        MENGINE_ASSERTION_FATAL( dataInfo->channels == 2, "invalid channels %u must be %u"
            , dataInfo->channels
            , 2
        );

        m_channels = dataInfo->channels;
        m_length = dataInfo->length;

        if( m_channels == 1 )
        {
            m_format = AL_FORMAT_MONO16;
            // Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
            //m_bufferSize = m_frequency >> 1;
            // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
            //m_bufferSize -= (m_bufferSize % 2);
            m_isStereo = false;
        }
        else if( m_channels == 2 )
        {
            m_format = AL_FORMAT_STEREO16;
            // Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
            //m_bufferSize = m_frequency;
            // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
            //m_bufferSize -= (m_bufferSize % 4);
            m_isStereo = true;
        }
        else if( m_channels == 4 )
        {
            m_format = alGetEnumValue( "AL_FORMAT_QUAD16" );
            // Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
            //m_bufferSize = m_frequency * 2;
            // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
            //m_bufferSize -= (m_bufferSize % 8);
            m_isStereo = true;
        }
        else if( m_channels == 6 )
        {
            m_format = alGetEnumValue( "AL_FORMAT_51CHN16" );
            // Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
            //m_bufferSize = m_frequency * 3;
            // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
            //m_bufferSize -= (m_bufferSize % 12);
            m_isStereo = true;
        }
        else
        {
            LOGGER_ERROR( "invalid channels %d"
                , m_channels
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::playSource( ALuint _source, bool _looped, float _pos )
    {
        m_sourceId = _source;
        m_looped = _looped;

        if( _pos > m_length )
        {
            LOGGER_ERROR( "pos %f > length %f"
                , _pos
                , m_length
            );

            return false;
        }

        ALint state = 0;
        OPENAL_CALL( alGetSourcei, (m_sourceId, AL_SOURCE_STATE, &state) );

        if( state != AL_STOPPED && state != AL_INITIAL )
        {
            LOGGER_ERROR( "source %d invalid state %d"
                , m_sourceId
                , state
            );

            return false;
        }

        OPENAL_CALL( alSourceRewind, (m_sourceId) );
        OPENAL_CALL( alSourcei, (m_sourceId, AL_BUFFER, 0) );
        OPENAL_CALL( alSourcei, (m_sourceId, AL_LOOPING, AL_FALSE) );

        if( m_soundDecoder->seek( _pos ) == false )
        {
            LOGGER_ERROR( "invalid seek '%f'"
                , _pos
            );

            return false;
        }

        for( uint32_t i = 0; i != MENGINE_OPENAL_STREAM_BUFFER_COUNT; ++i )
        {
            ALuint bufferId = m_alBuffersId[i];

            size_t bytesWritten;
            if( this->bufferData_( bufferId, &bytesWritten ) == false )
            {
                return false;
            }

            if( bytesWritten == 0 )
            {
                break;
            }

            OPENAL_CALL( alSourceQueueBuffers, (m_sourceId, 1, &bufferId) );

            if( bytesWritten != MENGINE_OPENAL_STREAM_BUFFER_SIZE )
            {
                break;
            }
        }

        OPENAL_CALL( alSourcePlay, (m_sourceId) );

        this->setUpdating_( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::resumeSource( ALuint _source )
    {
        MENGINE_UNUSED( _source );

        this->setUpdating_( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::pauseSource( ALuint _source )
    {
        MENGINE_UNUSED( _source );

        this->setUpdating_( false );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::stopSource( ALuint _source )
    {
        MENGINE_UNUSED( _source );

        this->setUpdating_( false );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::setUpdating_( bool _updating )
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
    bool OpenALSoundBufferStream::setTimePos( ALuint _source, float _pos ) const
    {
        MENGINE_UNUSED( _source );

        bool result = m_soundDecoder->seek( _pos );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::getTimePos( ALuint _source, float * const _pos ) const
    {
        MENGINE_UNUSED( _source );

        float timeTell = m_soundDecoder->tell();

        if( timeTell > m_length )
        {
            LOGGER_ERROR( "get tell %f > length %f"
                , timeTell
                , m_length
            );

            return false;
        }

        *_pos = timeTell;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::update()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

        if( m_updating == false )
        {
            return true;
        }

#ifdef MENGINE_DEBUG
        float newVolume;
        OPENAL_CALL( alGetSourcef, (m_sourceId, AL_GAIN, &newVolume) );
#endif

        ALint processed_count = 0;
        OPENAL_CALL( alGetSourcei, (m_sourceId, AL_BUFFERS_PROCESSED, &processed_count) );

        while( processed_count-- )
        {
            ALuint bufferId;
            OPENAL_CALL( alSourceUnqueueBuffers, (m_sourceId, 1, &bufferId) );

            size_t bytesWritten;
            this->bufferData_( bufferId, &bytesWritten );

            if( bytesWritten == 0 )
            {
                continue;
            }

            OPENAL_CALL( alSourceQueueBuffers, (m_sourceId, 1, &bufferId) );
        }

        ALint state;
        OPENAL_CALL( alGetSourcei, (m_sourceId, AL_SOURCE_STATE, &state) );

        if( state == AL_STOPPED )
        {
            if( m_looped == false )
            {
                ALint queued_count = 0;
                OPENAL_CALL( alGetSourcei, (m_sourceId, AL_BUFFERS_QUEUED, &queued_count) );

                if( queued_count == 0 )
                {
                    return false;
                }
            }

            OPENAL_CALL( alSourcePlay, (m_sourceId) );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::bufferData_( ALuint _alBufferId, size_t * const _bytes )
    {
        uint8_t * dataBuffer = m_memory->getBuffer();

        size_t bytesWritten = m_soundDecoder->decode( dataBuffer, MENGINE_OPENAL_STREAM_BUFFER_SIZE );

        if( bytesWritten != MENGINE_OPENAL_STREAM_BUFFER_SIZE && m_looped == true )
        {
            m_soundDecoder->rewind();

            size_t bytesWritten2 = m_soundDecoder->decode( dataBuffer + bytesWritten, MENGINE_OPENAL_STREAM_BUFFER_SIZE - bytesWritten );

            bytesWritten += bytesWritten2;
        }
        else if( bytesWritten == 0 )
        {
            *_bytes = 0;

            return true;
        }

        ALsizei al_bytesWritten = (ALsizei)bytesWritten;
        IF_OPENAL_CALL( alBufferData, (_alBufferId, m_format, dataBuffer, al_bytesWritten, m_frequency) )
        {
            LOGGER_ERROR( "buffer=%u id=%u format=%u bytes=%zu frequency=%d"
                , _alBufferId
                , m_sourceId
                , m_format
                , bytesWritten
                , m_frequency
            );

            return false;
        }

        *_bytes = bytesWritten;

        return true;
    }
}
