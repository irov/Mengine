#include "OpenALSoundBufferStream.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "OpenALSoundErrorHelper.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferStream::OpenALSoundBufferStream()
        : m_sourceId( AL_NONE )
        , m_looped( false )
        , m_updating( false )
    {
        StdAlgorithm::fill_n( m_alBuffersId, MENGINE_OPENAL_STREAM_BUFFER_COUNT, AL_NONE );
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferStream::~OpenALSoundBufferStream()
    {
#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        for( ALuint index = 0; index != MENGINE_OPENAL_STREAM_BUFFER_COUNT; ++index )
        {
            ALuint id = m_alBuffersId[index];

            MENGINE_ASSERTION_FATAL( id == AL_NONE, "buffer not released [%u]"
                , id
            );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::_acquireSoundBuffer()
    {
        ThreadMutexInterfacePtr mutexUpdating = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexUpdating, "invalid create mutex" );

        m_mutexUpdating = mutexUpdating;

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        memory->newBuffer( MENGINE_OPENAL_STREAM_BUFFER_SIZE );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::_releaseSoundBuffer()
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

            if( id == AL_NONE )
            {
                continue;
            }

            m_alBuffersId[i] = AL_NONE;

            this->releaseBufferId( id );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        for( uint32_t i = 0; i != MENGINE_OPENAL_STREAM_BUFFER_COUNT; ++i )
        {
            ALuint id = this->genBufferId();

            if( id == AL_NONE )
            {
                LOGGER_ASSERTION( "invalid gen %u buffer ID"
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
        m_duration = dataInfo->duration;

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
            LOGGER_ERROR( "invalid channels %u"
                , m_channels
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::playSource( ALuint _sourceId, bool _looped, float _position )
    {
        MENGINE_ASSERTION_FATAL( m_sourceId == AL_NONE, "source already playing %u"
            , m_sourceId
        );

        m_sourceId = _sourceId;
        m_looped = _looped;

        if( _position > m_duration )
        {
            LOGGER_ASSERTION( "source: %u pos %f > length %f"
                , m_sourceId
                , _position
                , m_duration
            );

            return false;
        }

        ALint state = 0;
        MENGINE_OPENAL_CALL( alGetSourcei, (m_sourceId, AL_SOURCE_STATE, &state) );

        switch( state )
        {
        case AL_INITIAL:
            {
                //Empty
            }break;
        case AL_STOPPED:
            {
                //Empty
            }break;
        case AL_PLAYING:
        case AL_PAUSED:
            {
                LOGGER_ASSERTION( "source [%u] invalid state [%u]"
                    , m_sourceId
                    , state
                );

                return false;
            }break;
        }

        MENGINE_OPENAL_CALL( alSourceRewind, (m_sourceId) );
        MENGINE_OPENAL_CALL( alSourcei, (m_sourceId, AL_BUFFER, 0) );
        MENGINE_OPENAL_CALL( alSourcei, (m_sourceId, AL_LOOPING, AL_FALSE) );

        if( m_soundDecoder->seek( _position ) == false )
        {
            LOGGER_ASSERTION( "invalid seek '%f'"
                , _position
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

            MENGINE_OPENAL_CALL( alSourceQueueBuffers, (m_sourceId, 1, &bufferId) );

            if( bytesWritten != MENGINE_OPENAL_STREAM_BUFFER_SIZE )
            {
                break;
            }
        }

        MENGINE_OPENAL_CALL( alSourcePlay, (m_sourceId) );

        this->setUpdating_( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::stopSource( ALuint _sourceId )
    {
        MENGINE_UNUSED( _sourceId );

        MENGINE_ASSERTION_FATAL( m_sourceId == _sourceId, "invalid source: %u"
            , _sourceId
        );

        this->setUpdating_( false );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

        MENGINE_OPENAL_CALL( alSourceStop, (_sourceId) );

        ALint queued = 0;
        MENGINE_OPENAL_CALL( alGetSourcei, (_sourceId, AL_BUFFERS_QUEUED, &queued) );

        for( ALint i = 0; i != queued; ++i )
        {
            ALuint bufferId = AL_NONE;
            MENGINE_OPENAL_CALL( alSourceUnqueueBuffers, (_sourceId, 1, &bufferId) );
        }

        MENGINE_OPENAL_CALL( alSourcei, (_sourceId, AL_BUFFER, 0) );

        m_sourceId = AL_NONE;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::pauseSource( ALuint _sourceId )
    {
        MENGINE_UNUSED( _sourceId );

        MENGINE_ASSERTION_FATAL( m_sourceId == _sourceId, "invalid source: %u"
            , _sourceId
        );

        MENGINE_OPENAL_CALL( alSourcePause, (_sourceId) );

        this->setUpdating_( false );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::resumeSource( ALuint _sourceId )
    {
        MENGINE_UNUSED( _sourceId );

        MENGINE_ASSERTION_FATAL( m_sourceId == _sourceId, "invalid source: %u"
            , _sourceId
        );

        this->setUpdating_( true );

        MENGINE_OPENAL_CALL( alSourcePlay, (_sourceId) );
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
    bool OpenALSoundBufferStream::setTimePosition( ALuint _sourceId, float _position ) const
    {
        MENGINE_UNUSED( _sourceId );

        MENGINE_ASSERTION_FATAL( m_sourceId == _sourceId, "invalid source: %u"
            , _sourceId
        );

        bool result = m_soundDecoder->seek( _position );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::getTimePosition( ALuint _sourceId, float * const _position ) const
    {
        MENGINE_UNUSED( _sourceId );

        MENGINE_ASSERTION_FATAL( m_sourceId == _sourceId, "invalid source: %u"
            , _sourceId
        );

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
    bool OpenALSoundBufferStream::updateSoundBuffer()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexUpdating );

        if( m_updating == false )
        {
            return true;
        }

#if defined(MENGINE_DEBUG)
        ALfloat newVolume;
        MENGINE_OPENAL_CALL( alGetSourcef, (m_sourceId, AL_GAIN, &newVolume) );
#endif

        ALint processed_count = 0;
        MENGINE_OPENAL_CALL( alGetSourcei, (m_sourceId, AL_BUFFERS_PROCESSED, &processed_count) );

        while( processed_count-- )
        {
            ALuint bufferId;
            MENGINE_OPENAL_CALL( alSourceUnqueueBuffers, (m_sourceId, 1, &bufferId) );

            size_t bytesWritten = 0;
            if( this->bufferData_( bufferId, &bytesWritten ) == false )
            {
                continue;
            }

            if( bytesWritten == 0 )
            {
                continue;
            }

            MENGINE_OPENAL_CALL( alSourceQueueBuffers, (m_sourceId, 1, &bufferId) );
        }

        ALint state;
        MENGINE_OPENAL_CALL( alGetSourcei, (m_sourceId, AL_SOURCE_STATE, &state) );

        switch( state )
        {
        case AL_PLAYING:
            {
                //Empty
            }break;
        case AL_PAUSED:
            {
                //Empty
            }break;
        case AL_INITIAL:
        case AL_STOPPED:
            {
                if( m_looped == false )
                {
                    ALint queued_count = 0;
                    MENGINE_OPENAL_CALL( alGetSourcei, (m_sourceId, AL_BUFFERS_QUEUED, &queued_count) );

                    if( queued_count == 0 )
                    {
                        return false;
                    }
                }

                MENGINE_IF_OPENAL_CALL( alSourcePlay, (m_sourceId) )
                {
                    return false;
                }
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::bufferData_( ALuint _alBufferId, size_t * const _bytes )
    {
        uint8_t * dataBuffer = m_memory->getBuffer();

        SoundDecoderData decoder_data_body;
        decoder_data_body.buffer = dataBuffer;
        decoder_data_body.size = MENGINE_OPENAL_STREAM_BUFFER_SIZE;

        size_t bytesWritten = m_soundDecoder->decode( &decoder_data_body );

        if( m_looped == true && bytesWritten != MENGINE_OPENAL_STREAM_BUFFER_SIZE )
        {
            if( m_soundDecoder->rewind() == false )
            {
                return false;
            }

            size_t bufferSize = MENGINE_OPENAL_STREAM_BUFFER_SIZE - bytesWritten;

            bufferSize -= bufferSize % 4;

            SoundDecoderData decoder_data_tail;
            decoder_data_tail.buffer = dataBuffer + bytesWritten;
            decoder_data_tail.size = bufferSize;

            size_t bytesWritten2 = m_soundDecoder->decode( &decoder_data_tail );

            bytesWritten += bytesWritten2;
        }

        if( bytesWritten == 0 )
        {
            *_bytes = 0;

            return true;
        }

        ALsizei al_bytesWritten = (ALsizei)bytesWritten;
        ALsizei al_frequency = (ALsizei)m_frequency;
        MENGINE_IF_OPENAL_CALL( alBufferData, (_alBufferId, m_format, dataBuffer, al_bytesWritten, al_frequency) )
        {
            LOGGER_ASSERTION( "invalid alBufferData buffer: %u source: %u format: %u bytes: %zu frequency: %u"
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
    //////////////////////////////////////////////////////////////////////////
}
