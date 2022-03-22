#include "SDLMutexFileInputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"

#ifdef MENGINE_DEBUG
#   include "Interface/NotificationServiceInterface.h"
#   include "Interface/PlatformInterface.h"
#endif

#include "Kernel/Logger.h"
#include "Kernel/ThreadMutexScope.h"

#include "SDLFileHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    SDLMutexFileInputStream::SDLMutexFileInputStream()
        : m_size( 0 )
        , m_offset( 0 )
        , m_carriage( 0 )
        , m_capacity( 0 )
        , m_reading( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLMutexFileInputStream::~SDLMutexFileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::initialize( const SDLFileInputStreamPtr & _stream, const ThreadMutexInterfacePtr & _mutex )
    {
        m_stream = _stream;
        m_mutex = _mutex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_UNUSED( _streaming );
        MENGINE_UNUSED( _share );

#ifdef MENGINE_DEBUG
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;
#endif

        size_t size = m_stream->size();

        if( _size != ~0U )
        {
            if( _offset + _size > size )
            {
                LOGGER_ERROR( "invalid relation '%s' folder '%s' file '%s' range %zu:%zu size %zu"
                    , _relationPath.c_str()
                    , _folderPath.c_str()
                    , _filePath.c_str()
                    , _offset
                    , _size
                    , (size_t)size
                );

                this->close();

                return false;
            }

            m_size = _size;
        }
        else
        {
            m_size = size;
        }

        m_offset = _offset;

        m_carriage = 0;
        m_capacity = 0;
        m_reading = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::close()
    {
        m_stream = nullptr;
        m_mutex = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLMutexFileInputStream::read( void * const _buf, size_t _count )
    {
        size_t pos = m_reading - m_capacity + m_carriage;

        size_t correct_count = _count;

        if( pos + _count > m_size )
        {
            correct_count = m_size - pos;
        }

        if( correct_count > MENGINE_FILE_STREAM_BUFFER_SIZE )
        {
            size_t tail = m_capacity - m_carriage;

            if( tail != 0 )
            {
                stdex::memorycopy( _buf, 0, m_readCache + m_carriage, tail );
            }

            size_t toRead = correct_count - tail;
            
            size_t bytesRead;
            if( this->read_( _buf, tail, toRead, &bytesRead ) == false )
            {
                return 0;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading += bytesRead;

            return bytesRead + tail;
        }

        if( m_carriage + correct_count <= m_capacity )
        {
            stdex::memorycopy( _buf, 0, m_readCache + m_carriage, correct_count );

            m_carriage += correct_count;

            return correct_count;
        }

        size_t tail = m_capacity - m_carriage;

        if( tail != 0 )
        {
            stdex::memorycopy( _buf, 0, m_readCache + m_carriage, tail );
        }

        size_t bytesRead;
        if( this->read_( m_readCache, 0, MENGINE_FILE_STREAM_BUFFER_SIZE, &bytesRead ) == false )
        {
            return 0;
        }

        size_t readSize = MENGINE_MIN( correct_count - tail, bytesRead );

        stdex::memorycopy( _buf, tail, m_readCache, readSize );

        m_carriage = readSize;
        m_capacity = bytesRead;

        m_reading += bytesRead;

        return readSize + tail;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        SDL_RWops * rwops = m_stream->getRWops();

        size_t current = m_reading - m_capacity + m_carriage;

        size_t pos = m_offset + current;

        Sint64 result = SDL_RWseek( rwops, static_cast<Sint64>(pos), RW_SEEK_SET );

        if( 0 > result )
        {
            LOGGER_ERROR( "seek %zu:%zu get [error: %s]"
                , pos
                , m_size
                , SDL_GetError()
            );

            return false;
        }

        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        size_t bytesRead = SDL_RWread( rwops, buf_offset, 1, _size );

        *_read = bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::seek( size_t _pos )
    {
        size_t carriage = _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLMutexFileInputStream::rewind()
    {
        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::rseek( size_t _pos )
    {
        size_t carriage = m_size - _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::skip( size_t _size )
    {
        size_t current = m_reading - m_capacity + m_carriage;

        size_t carriage = current + _size;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::seek_( size_t _pos )
    {
        if( _pos >= m_reading - m_capacity && _pos <= m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
            m_carriage = 0;
            m_capacity = 0;

            m_reading = _pos;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLMutexFileInputStream::tell() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLMutexFileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::eof() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::time( uint64_t * const _time ) const
    {
#ifdef MENGINE_DEBUG
        Char filePath[MENGINE_MAX_PATH] = {'\0'};
        if( Helper::concatenateFilePath( m_relationPath, m_folderPath, m_filePath, filePath, MENGINE_MAX_PATH - 1 ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            return false;
        }

        uint64_t ft = PLATFORM_SERVICE()
            ->getFileTime( filePath );

        *_time = ft;

        return true;
#else
        MENGINE_UNUSED( _time );

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMutexFileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
    //////////////////////////////////////////////////////////////////////////
    const FilePath & SDLMutexFileInputStream::getRelationPath() const
    {
        return m_relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & SDLMutexFileInputStream::getFolderPath() const
    {
        return m_folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & SDLMutexFileInputStream::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
}
