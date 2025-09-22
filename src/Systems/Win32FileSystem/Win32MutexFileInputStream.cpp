#include "Win32MutexFileInputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Environment/Windows/Win32Helper.h"
#include "Environment/Windows/Win32FileHelper.h"
#include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"

#include "Win32ConcatenateFileHelper.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/DebugFileHelper.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    Win32MutexFileInputStream::Win32MutexFileInputStream()
        : m_size( 0 )
        , m_offset( 0 )
        , m_carriage( 0 )
        , m_capacity( 0 )
        , m_reading( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32MutexFileInputStream::~Win32MutexFileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MutexFileInputStream::setFileInputStream( const Win32FileInputStreamPtr & _stream )
    {
        m_stream = _stream;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MutexFileInputStream::setThreadMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        m_mutex = _mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MutexFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_UNUSED( _relationPath );
        MENGINE_UNUSED( _folderPath );
        MENGINE_UNUSED( _filePath );
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
                    , size
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
    void Win32MutexFileInputStream::close()
    {
        m_stream = nullptr;
        m_mutex = nullptr;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32MutexFileInputStream::read( void * const _buf, size_t _count )
    {
        size_t pos = m_reading - m_capacity + m_carriage;

        size_t correct_count = _count;

        if( pos + _count > m_size )
        {
            correct_count = m_size - pos;
        }

        if( correct_count > MENGINE_WIN32_FILE_STREAM_BUFFER_SIZE )
        {
            size_t tail = m_capacity - m_carriage;

            if( tail != 0 )
            {
                Helper::memoryCopy( _buf, 0, m_readCache, m_carriage, tail );
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
            Helper::memoryCopy( _buf, 0, m_readCache, m_carriage, correct_count );

            m_carriage += correct_count;

            return correct_count;
        }

        size_t tail = m_capacity - m_carriage;

        if( tail != 0 )
        {
            Helper::memoryCopy( _buf, 0, m_readCache, m_carriage, tail );
        }

        size_t bytesRead;
        if( this->read_( m_readCache, 0, MENGINE_WIN32_FILE_STREAM_BUFFER_SIZE, &bytesRead ) == false )
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
    bool Win32MutexFileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        HANDLE hFile = m_stream->getHandleFile();

        size_t current = m_reading - m_capacity + m_carriage;

        size_t pos = m_offset + current;

        LARGE_INTEGER liDistanceToMove;
        liDistanceToMove.QuadPart = pos;

        LARGE_INTEGER dwPtr;
        if( ::SetFilePointerEx( hFile, liDistanceToMove, &dwPtr, FILE_BEGIN ) == FALSE )
        {
            LOGGER_ERROR( "seek %zu:%zu get %ls"
                , pos
                , m_size
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        DWORD bytesRead = 0;
        if( ::ReadFile( hFile, buf_offset, static_cast<DWORD>(_size), &bytesRead, NULL ) == FALSE )
        {
            LOGGER_ERROR( "read offset %zu size %zu:%zu %ls"
                , _offset
                , _size
                , m_size
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        *_read = (size_t)bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MutexFileInputStream::seek( size_t _carriage )
    {
        size_t carriage = _carriage;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MutexFileInputStream::rewind()
    {
        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MutexFileInputStream::rseek( size_t _carriage )
    {
        size_t carriage = m_size - _carriage;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MutexFileInputStream::skip( size_t _offset )
    {
        size_t current = m_reading - m_capacity + m_carriage;

        size_t carriage = current + _offset;

        bool result = this->seek_( carriage );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MutexFileInputStream::seek_( size_t _carriage )
    {
        if( _carriage >= m_reading - m_capacity && _carriage <= m_reading )
        {
            m_carriage = m_capacity - (m_reading - _carriage);
        }
        else
        {
            m_carriage = 0;
            m_capacity = 0;

            m_reading = _carriage;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32MutexFileInputStream::tell() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32MutexFileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MutexFileInputStream::eof() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MutexFileInputStream::time( uint64_t * const _time ) const
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        FILETIME creation;
        FILETIME access;
        FILETIME write;

        HANDLE hFile = m_stream->getHandleFile();

        if( ::GetFileTime( hFile, &creation, &access, &write ) == FALSE )
        {
            LOGGER_ERROR( "invalid GetFileTime get error %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        time_t time = Helper::Win32FileTimeToUnixTime( &write );

        *_time = (uint64_t)time;

        return true;
#else
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MutexFileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
