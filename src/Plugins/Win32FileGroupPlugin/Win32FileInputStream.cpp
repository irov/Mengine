#include "Win32FileInputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/Win32PlatformExtensionInterface.h"

#include "Win32FileHelper.h"

#include "Kernel/Win32Helper.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#include "stdex/memorycopy.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    Win32FileInputStream::Win32FileInputStream()
        : m_hFile( INVALID_HANDLE_VALUE )
        , m_size( 0 )
        , m_offset( 0 )
        , m_carriage( 0 )
        , m_capacity( 0 )
        , m_reading( 0 )
        , m_streaming( false )
        , m_share( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileInputStream::~Win32FileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileInputStream, this, "Win32FileInputStream::open" );

#ifdef MENGINE_DEBUG
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;
#endif

        m_streaming = _streaming;
        m_share = _share;

        WChar fullPath[MENGINE_MAX_PATH] = {L'\0'};
        if( this->openFile_( _relationPath, _folderPath, _filePath, fullPath ) == false )
        {
            return false;
        }

        LARGE_INTEGER lpFileSize;
        if( ::GetFileSizeEx( m_hFile, &lpFileSize ) == FALSE )
        {
            LOGGER_ERROR( "invalid file '%ls' size %s"
                , fullPath
                , Helper::Win32GetLastErrorMessage()
            );

            this->close();

            return false;
        }

        size_t size = (size_t)lpFileSize.QuadPart;

        if( _size != ~0U )
        {
            if( _offset + _size > size )
            {
                LOGGER_ERROR( "invalid file '%ls' range %zu:%zu size %zu"
                    , fullPath
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

        if( m_offset != 0 )
        {
            LARGE_INTEGER liDistanceToMove;
            liDistanceToMove.QuadPart = m_offset;

            LARGE_INTEGER dwPtr;
            if( ::SetFilePointerEx( m_hFile, liDistanceToMove, &dwPtr, FILE_BEGIN ) == FALSE )
            {
                LOGGER_ERROR( "seek offset %zu size %zu %s"
                    , m_offset
                    , m_size
                    , Helper::Win32GetLastErrorMessage()
                );

                this->close();

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::close()
    {
        if( m_hFile == INVALID_HANDLE_VALUE )
        {
            return true;
        }

#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, m_folderPath.c_str(), m_filePath.c_str(), m_streaming );
        }
#endif

        BOOL successful = ::CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;

        if( successful == FALSE )
        {
            LOGGER_ERROR( "invalid close '%s:%s' handle %s"
                , MENGINE_DEBUG_VALUE( m_folderPath.c_str(), "" )
                , MENGINE_DEBUG_VALUE( m_filePath.c_str(), "" )
                , Helper::Win32GetLastErrorMessage()
            );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * const _fullPath )
    {
        size_t fullPathLen = Helper::Win32ConcatenateFilePathW( _relationPath, _folderPath, _filePath, _fullPath, MENGINE_MAX_PATH );

        MENGINE_UNUSED( fullPathLen );

        MENGINE_ASSERTION_FATAL( fullPathLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePath '%s':'%s'"
            , _folderPath.c_str()
            , _filePath.c_str()
        );

        DWORD sharedMode = FILE_SHARE_READ;

        if( m_share == true )
        {
            sharedMode |= FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        }

        HANDLE hFile = Helper::Win32CreateFile(
            _fullPath, // file to open
            GENERIC_READ, // open for reading
            sharedMode, // share for reading, exclusive for mapping
            OPEN_EXISTING // existing file only
        );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "file '%ls' invalid open"
                , _fullPath
            );

            return false;
        }

        m_hFile = hFile;

#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, _folderPath.c_str(), _filePath.c_str(), m_streaming );
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileInputStream::read( void * const _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileInputStream, this, "Win32FileInputStream::read" );

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
        if( this->read_( m_readCache, 0, MENGINE_WIN32_FILE_STREAM_BUFFER_SIZE, &bytesRead ) == false )
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
    bool Win32FileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        DWORD bytesRead = 0;
        if( ::ReadFile( m_hFile, buf_offset, static_cast<DWORD>(_size), &bytesRead, NULL ) == FALSE )
        {
            LOGGER_ERROR( "read offset %zu size %zu:%zu %s"
                , _offset
                , _size
                , m_size
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        *_read = (size_t)bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::seek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileInputStream, this, "Win32FileInputStream::seek" );

        bool successful = this->seek_( _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::rseek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileInputStream, this, "Win32FileInputStream::rseek" );

        bool successful = this->seek_( m_size - _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::seek_( size_t _pos )
    {
        if( _pos >= m_reading - m_capacity && _pos <= m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
            LARGE_INTEGER liDistanceToMove;
            liDistanceToMove.QuadPart = m_offset + _pos;

            LARGE_INTEGER dwPtr;
            if( ::SetFilePointerEx( m_hFile, liDistanceToMove, &dwPtr, FILE_BEGIN ) == FALSE )
            {
                LOGGER_ERROR( "seek %zu:%zu get %s"
                    , _pos
                    , m_size
                    , Helper::Win32GetLastErrorMessage()
                );

                return false;
            }

            m_carriage = 0;
            m_capacity = 0;

            size_t dwPtrSizet = static_cast<size_t>(dwPtr.QuadPart);

            m_reading = dwPtrSizet - m_offset;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::skip( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileInputStream, this, "Win32FileInputStream::skip" );

        size_t current = m_reading - m_capacity + m_carriage;

        size_t seek_pos = current + _pos;

        bool result = this->seek_( seek_pos );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileInputStream::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileInputStream, this, "Win32FileInputStream::tell" );

        size_t current = m_reading - m_capacity + m_carriage;

        return current;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileInputStream, this, "Win32FileInputStream::eof" );

        size_t current = m_reading - m_capacity + m_carriage;

        return current == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::time( uint64_t * const _time ) const
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        FILETIME creation;
        FILETIME access;
        FILETIME write;

        if( ::GetFileTime( m_hFile, &creation, &access, &write ) == FALSE )
        {
            LOGGER_ERROR( "invalid get file time %s"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getUnknown();

        time_t time = win32Platform->getFileUnixTime( &write );

        *_time = (uint64_t)time;

        return true;
#else
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    HANDLE Win32FileInputStream::getHandleFile() const
    {
        return m_hFile;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Win32FileInputStream::getRelationPath() const
    {
        return m_relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Win32FileInputStream::getFolderPath() const
    {
        return m_folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Win32FileInputStream::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
}
