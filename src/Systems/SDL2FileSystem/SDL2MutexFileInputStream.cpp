#include "SDL2MutexFileInputStream.h"

#include "Interface/FileSystemInterface.h"

#if defined(MENGINE_DEBUG)
#   include "Interface/PlatformServiceInterface.h"
#endif

#include "Kernel/Logger.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/PathHelper.h"

#include "Config/Path.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    SDL2MutexFileInputStream::SDL2MutexFileInputStream()
        : m_size( 0 )
        , m_offset( 0 )
        , m_carriage( 0 )
        , m_capacity( 0 )
        , m_reading( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2MutexFileInputStream::~SDL2MutexFileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2MutexFileInputStream::initialize( const SDL2FileInputStreamPtr & _stream, const ThreadMutexInterfacePtr & _mutex )
    {
        m_stream = _stream;
        m_mutex = _mutex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2MutexFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_UNUSED( _streaming );
        MENGINE_UNUSED( _share );

#if defined(MENGINE_DEBUG)
        this->setDebugRelationPath( _relationPath );
        this->setDebugFolderPath( _folderPath );
        this->setDebugFilePath( _filePath );
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
    bool SDL2MutexFileInputStream::close()
    {
        m_stream = nullptr;
        m_mutex = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL2MutexFileInputStream::read( void * const _buf, size_t _count )
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
    bool SDL2MutexFileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
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
    bool SDL2MutexFileInputStream::seek( size_t _pos )
    {
        size_t carriage = _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2MutexFileInputStream::rewind()
    {
        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2MutexFileInputStream::rseek( size_t _pos )
    {
        size_t carriage = m_size - _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2MutexFileInputStream::skip( size_t _size )
    {
        size_t current = m_reading - m_capacity + m_carriage;

        size_t carriage = current + _size;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2MutexFileInputStream::seek_( size_t _pos )
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
    size_t SDL2MutexFileInputStream::tell() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL2MutexFileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2MutexFileInputStream::eof() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2MutexFileInputStream::time( uint64_t * const _time ) const
    {
#if defined(MENGINE_DEBUG)
        const FilePath & relationPath = this->getDebugRelationPath();
        const FilePath & folderPath = this->getDebugFolderPath();
        const FilePath & filePath = this->getDebugFilePath();

        Path fullPath = {'\0'};
        if( Helper::concatenateFilePath( {relationPath, folderPath, filePath}, fullPath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                , folderPath.c_str()
                , filePath.c_str()
            );

            return false;
        }

        uint64_t ft = FILE_SYSTEM()
            ->getFileTime( fullPath );

        *_time = ft;

        return true;
#else
        MENGINE_UNUSED( _time );

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2MutexFileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
