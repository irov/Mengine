#include "POSIXMutexFileInputStream.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/DebugFileHelper.h"
#include "Kernel/DocumentHelper.h"

#include "Kernel/MemoryCopy.h"

#include <cerrno>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    POSIXMutexFileInputStream::POSIXMutexFileInputStream()
        : m_size( 0 )
        , m_offset( 0 )
        , m_carriage( 0 )
        , m_capacity( 0 )
        , m_reading( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXMutexFileInputStream::~POSIXMutexFileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXMutexFileInputStream::setFileInputStream( const POSIXFileInputStreamPtr & _stream )
    {
        m_stream = _stream;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXMutexFileInputStream::setThreadMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        m_mutex = _mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXMutexFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_UNUSED( _streaming );
        MENGINE_UNUSED( _share );

        size_t size = m_stream->size();

        if( _offset > size )
        {
            LOGGER_ERROR( "invalid relation '%s' folder '%s' file '%s' offset %zu size %zu"
                , _relationPath.c_str()
                , _folderPath.c_str()
                , _filePath.c_str()
                , _offset
                , size
            );

            this->close();

            return false;
        }

        if( _size != MENGINE_UNKNOWN_SIZE )
        {
            if( _size > size - _offset )
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
            m_size = size - _offset;
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
    void POSIXMutexFileInputStream::close()
    {
        m_stream = nullptr;
        m_mutex = nullptr;

        m_size = 0;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t POSIXMutexFileInputStream::read( void * const _buf, size_t _count )
    {
        size_t pos = m_reading - m_capacity + m_carriage;

        if( pos >= m_size )
        {
            return 0;
        }

        size_t correct_count = _count;

        if( _count > m_size - pos )
        {
            correct_count = m_size - pos;
        }

        if( correct_count > MENGINE_FILE_STREAM_BUFFER_SIZE )
        {
            size_t tail = m_capacity - m_carriage;

            if( tail != 0 )
            {
                Helper::memoryCopy( _buf, 0, m_readCache + m_carriage, 0, tail );
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
            Helper::memoryCopy( _buf, 0, m_readCache + m_carriage, 0, correct_count );

            m_carriage += correct_count;

            return correct_count;
        }

        size_t tail = m_capacity - m_carriage;

        if( tail != 0 )
        {
            Helper::memoryCopy( _buf, 0, m_readCache + m_carriage, 0, tail );
        }

        size_t bytesRead;
        if( this->read_( m_readCache, 0, MENGINE_FILE_STREAM_BUFFER_SIZE, &bytesRead ) == false )
        {
            return 0;
        }

        size_t readSize = MENGINE_MIN( correct_count - tail, bytesRead );

        Helper::memoryCopy( _buf, tail, m_readCache, 0, readSize );

        m_carriage = readSize;
        m_capacity = bytesRead;

        m_reading += bytesRead;

        return readSize + tail;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXMutexFileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        FILE * file = m_stream->getFILE();

        size_t current = m_reading - m_capacity + m_carriage;

        size_t pos = m_offset + current;

        int32_t result = ::fseeko( file, pos, SEEK_SET );

        if( result < 0 )
        {
            LOGGER_ERROR( "invalid seek pos: %zu size: %zu"
                , pos
                , m_size
            );

            return false;
        }

        ::clearerr( file );

        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        size_t bytesRead = ::fread( buf_offset, 1, _size, file );

        if( bytesRead == 0 && ::ferror( file ) != 0 )
        {
            LOGGER_ERROR( "invalid read pos: %zu size: %zu error: %d"
                , pos
                , _size
                , errno
            );

            return false;
        }

        *_read = bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXMutexFileInputStream::seek( size_t _pos )
    {
        size_t carriage = _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXMutexFileInputStream::rewind()
    {
        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXMutexFileInputStream::rseek( size_t _pos )
    {
        if( _pos > m_size )
        {
            return false;
        }

        size_t carriage = m_size - _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXMutexFileInputStream::skip( size_t _size )
    {
        size_t current = m_reading - m_capacity + m_carriage;

        if( _size > m_size - current )
        {
            return false;
        }

        size_t carriage = current + _size;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXMutexFileInputStream::seek_( size_t _pos )
    {
        if( _pos > m_size )
        {
            return false;
        }

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
    size_t POSIXMutexFileInputStream::tell() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t POSIXMutexFileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXMutexFileInputStream::eof() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        if( carriage >= m_size )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXMutexFileInputStream::time( uint64_t * const _time ) const
    {
        return m_stream->time( _time );
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXMutexFileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
