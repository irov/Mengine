#include "OpenALSoundBufferStream.h"
#include "OpenALSoundSystem.h"
#include "OpenALSoundError.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferStream::OpenALSoundBufferStream()
        : m_sourceId( 0 )
        , m_looped( false )
        , m_updating( false )
    {
        for( uint32_t i = 0; i != MENGINE_OPENAL_STREAM_BUFFER_COUNT; ++i )
        {
            m_alBuffersId[i] = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferStream::~OpenALSoundBufferStream()
    {
        this->removeBuffers_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::_initialize()
    {
        ThreadMutexInterfacePtr mutexUpdating = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        if( mutexUpdating == nullptr )
        {
            return false;
        }

        m_mutexUpdating = mutexUpdating;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::removeBuffers_()
    {
        this->setUpdating_( false );

        for( uint32_t i = 0; i != MENGINE_OPENAL_STREAM_BUFFER_COUNT; ++i )
        {
            ALuint id = m_alBuffersId[i];

            if( id != 0 )
            {
                m_soundSystem->releaseBufferId( id );
            }

            m_alBuffersId[i] = 0;
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
                LOGGER_ERROR( "invalid gen %d buffer ID"
                    , i
                );

                this->removeBuffers_();

                return false;
            }

            m_alBuffersId[i] = id;
        }

        m_soundDecoder = _soundDecoder;

        const SoundCodecDataInfo* dataInfo = m_soundDecoder->getCodecDataInfo();
        m_frequency = dataInfo->frequency;

        if( dataInfo->channels != 2 )
        {
            LOGGER_ERROR( "invalid channels %d must be %d"
                , dataInfo->channels
                , 2
            );

            return false;
        }

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
    bool OpenALSoundBufferStream::play( ALuint _source, bool _looped, float _pos )
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
        alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
        OPENAL_CHECK_ERROR();

        if( state != AL_STOPPED && state != AL_INITIAL )
        {
            this->stop( m_sourceId );
            //alSourceRewind( _source );
        }

        alSourcei( m_sourceId, AL_BUFFER, 0 ); // clear source buffering
        OPENAL_CHECK_ERROR();

        alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );
        OPENAL_CHECK_ERROR();

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
            if( this->bufferData_( bufferId, bytesWritten ) == false )
            {
                return false;
            }

            if( bytesWritten == 0 )
            {
                break;
            }

            alSourceQueueBuffers( m_sourceId, 1, &bufferId );
            OPENAL_CHECK_ERROR();

            if( bytesWritten != MENGINE_OPENAL_STREAM_BUFFER_SIZE )
            {
                break;
            }
        }

        alSourcePlay( m_sourceId );
        OPENAL_CHECK_ERROR();

        this->setUpdating_( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::resume( ALuint _source )
    {
        (void)_source;

        alSourcePlay( m_sourceId );
        OPENAL_CHECK_ERROR();

        this->setUpdating_( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::pause( ALuint _source )
    {
        this->setUpdating_( false );

        alSourcePause( _source );
        OPENAL_CHECK_ERROR();
        //m_soundSystem->clearSourceId( _source );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::stop( ALuint _source )
    {
        this->setUpdating_( false );

        ALint process_count = 0;
        // Получаем количество отработанных буферов
        alGetSourcei( _source, AL_BUFFERS_PROCESSED, &process_count );
        OPENAL_CHECK_ERROR();

        // Если таковые существуют то
        while( process_count-- > 0 )
        {
            // Исключаем их из очереди
            ALuint buffer = 0;

            alSourceUnqueueBuffers( _source, 1, &buffer );
            OPENAL_CHECK_ERROR();
        }

        alSourceStop( _source );
        OPENAL_CHECK_ERROR();

        ALint queued_count = 0;
        // unqueue remaining buffers
        alGetSourcei( _source, AL_BUFFERS_QUEUED, &queued_count );
        OPENAL_CHECK_ERROR();

        while( queued_count-- > 0 )
        {
            // Исключаем их из очереди
            ALuint buffer = 0;

            alSourceUnqueueBuffers( _source, 1, &buffer );
            OPENAL_CHECK_ERROR();
        }

        alSourcei( m_sourceId, AL_BUFFER, 0 ); // clear source buffering

        alSourceRewind( _source );
        OPENAL_CHECK_ERROR();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferStream::setUpdating_( bool _updating )
    {
        m_mutexUpdating->lock();
        m_updating = _updating;
        m_mutexUpdating->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::getUpdating_() const
    {
        return m_updating;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::setTimePos( ALuint _source, float _pos ) const
    {
        (void)_source;

        bool result = m_soundDecoder->seek( _pos );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::getTimePos( ALuint _source, float & _pos ) const
    {
        (void)_source;

        float timeTell = m_soundDecoder->tell();

        if( timeTell > m_length )
        {
            LOGGER_ERROR( "get tell %f > length %f"
                , timeTell
                , m_length
            );

            return false;
        }

        _pos = timeTell;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::update()
    {
        ThreadMutexScope mutexUpdatingScope( m_mutexUpdating );

        if( this->m_updating == false )
        {
            return true;
        }

        ALint processed_count = 0;
        alGetSourcei( m_sourceId, AL_BUFFERS_PROCESSED, &processed_count );
        OPENAL_CHECK_ERROR();

        ALuint unqueueBuffersId[MENGINE_OPENAL_STREAM_BUFFER_COUNT];
        alSourceUnqueueBuffers( m_sourceId, processed_count, unqueueBuffersId );
        OPENAL_CHECK_ERROR();

        ALint queued_count = 0;
        alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queued_count );
        OPENAL_CHECK_ERROR();

        bool end = false;
        for( ALint curr_processed = 0; curr_processed != processed_count; ++curr_processed )
        {
            // Исключаем их из очереди
            ALuint bufferId = unqueueBuffersId[curr_processed];

            // Читаем очередную порцию данных
            size_t bytesWritten;
            this->bufferData_( bufferId, bytesWritten );

            if( bytesWritten == 0 )
            {
                if( m_looped == false )
                {
                    end = true;

                    continue;
                }
            }

            alSourceQueueBuffers( m_sourceId, 1, &bufferId );
            OPENAL_CHECK_ERROR();
        }

        if( end == true )
        {
            return false;
        }

        ALint state;
        alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
        OPENAL_CHECK_ERROR();

        if( state != AL_PLAYING && state != AL_PAUSED )
        {
            alSourcePlay( m_sourceId );
            OPENAL_CHECK_ERROR();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferStream::bufferData_( ALuint _alBufferId, size_t & _bytes )
    {
        char dataBuffer[MENGINE_OPENAL_STREAM_BUFFER_SIZE];
        size_t bytesWritten = m_soundDecoder->decode( dataBuffer, MENGINE_OPENAL_STREAM_BUFFER_SIZE );

        if( bytesWritten == 0 )
        {
            _bytes = 0;

            return true;
        }

        if( bytesWritten != MENGINE_OPENAL_STREAM_BUFFER_SIZE && m_looped == true )
        {
            m_soundDecoder->rewind();

            size_t bytesWritten2 = m_soundDecoder->decode( dataBuffer + bytesWritten, MENGINE_OPENAL_STREAM_BUFFER_SIZE - bytesWritten );

            bytesWritten += bytesWritten2;
        }

        ALsizei al_bytesWritten = (ALsizei)bytesWritten;
        alBufferData( _alBufferId, m_format, dataBuffer, al_bytesWritten, m_frequency );

        if( OPENAL_CHECK_ERROR() == false )
        {
            LOGGER_ERROR( "buffer=%d id=%d format=%d bytes=%d frequency=%d"
                , _alBufferId
                , m_format
                , bytesWritten
                , m_frequency
            );

            return false;
        }

        _bytes = bytesWritten;

        return true;
    }
}
