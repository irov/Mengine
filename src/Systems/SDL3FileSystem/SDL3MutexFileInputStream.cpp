#include "SDL3MutexFileInputStream.h"

#include "Interface/FileSystemInterface.h"

#if defined(MENGINE_DEBUG)
#   include "Interface/PlatformServiceInterface.h"
#endif

#include "Kernel/Logger.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/PathHelper.h"
#include "Kernel/DebugFileHelper.h"

#include "Config/Path.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    SDL3MutexFileInputStream::SDL3MutexFileInputStream()
        : m_size( 0 )
        , m_offset( 0 )
        , m_carriage( 0 )
        , m_capacity( 0 )
        , m_reading( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3MutexFileInputStream::~SDL3MutexFileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3MutexFileInputStream::initialize( const SDL3FileInputStreamPtr & _stream, const ThreadMutexInterfacePtr & _mutex )
    {
        m_stream = _stream;
        m_mutex = _mutex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3MutexFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_UNUSED( _streaming );
        MENGINE_UNUSED( _share );

        size_t size = m_stream->size();

        if( _size != MENGINE_UNKNOWN_SIZE )
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

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::addDebugFilePath( this, _relationPath, _folderPath, _filePath, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3MutexFileInputStream::close()
    {
        m_stream = nullptr;
        m_mutex = nullptr;

        m_size = 0;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL3MutexFileInputStream::read( void * const _buf, size_t _count )
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
    bool SDL3MutexFileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        SDL_IOStream * iostream = m_stream->getIOStream();

        size_t current = m_reading - m_capacity + m_carriage;

        size_t pos = m_offset + current;

        Sint64 result = SDL_SeekIO( iostream, static_cast<Sint64>(pos), SDL_IO_SEEK_SET );

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

        size_t bytesRead = SDL_ReadIO( iostream, buf_offset, _size );

        *_read = bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3MutexFileInputStream::seek( size_t _pos )
    {
        size_t carriage = _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3MutexFileInputStream::rewind()
    {
        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3MutexFileInputStream::rseek( size_t _pos )
    {
        size_t carriage = m_size - _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3MutexFileInputStream::skip( size_t _size )
    {
        size_t current = m_reading - m_capacity + m_carriage;

        size_t carriage = current + _size;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3MutexFileInputStream::seek_( size_t _pos )
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
    size_t SDL3MutexFileInputStream::tell() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL3MutexFileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3MutexFileInputStream::eof() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3MutexFileInputStream::time( uint64_t * const _time ) const
    {
#if defined(MENGINE_DEBUG)
        const FilePath & relationPath = Helper::getDebugRelationPath( this );
        const FilePath & folderPath = Helper::getDebugFolderPath( this );
        const FilePath & filePath = Helper::getDebugFilePath( this );

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
    bool SDL3MutexFileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}