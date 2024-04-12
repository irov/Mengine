#include "AndroidFileInputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/DebugFileHelper.h"

#include "stdex/memorycopy.h"

#include <sys/stat.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    AndroidFileInputStream::AndroidFileInputStream()
        : m_file( nullptr )
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
    AndroidFileInputStream::~AndroidFileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileInputStream, this, "AndroidFileInputStream::open" );

#if defined(MENGINE_DEBUG)
        this->setDebugRelationPath( _relationPath );
        this->setDebugFolderPath( _folderPath );
        this->setDebugFilePath( _filePath );
#endif

        m_streaming = _streaming;
        m_share = _share;

        Char fullPath[MENGINE_MAX_PATH] = {'\0'};
        if( this->openFile_( _relationPath, _folderPath, _filePath, fullPath ) == false )
        {
            return false;
        }

        ::fseek( m_file, 0, SEEK_END );
        int64_t size = ::ftell( m_file );
        ::rewind( m_file );

        if( size <= 0 )
        {
            LOGGER_ERROR( "invalid file '%s' error size %zu"
                , fullPath
                , (size_t)size
            );

            this->close();

            return false;
        }

        if( _size != ~0U )
        {
            if( _offset + _size > (size_t)size )
            {
                LOGGER_ERROR( "invalid file '%s' range %zu:%zu size %zu"
                    , fullPath
                    , _offset
                    , _size
                    , (size_t)size
                );

                return false;
            }

            m_size = _size;
        }
        else
        {
            m_size = (size_t)size;
        }

        m_offset = _offset;

        m_carriage = 0;
        m_capacity = 0;
        m_reading = 0;

        if( m_offset != 0 )
        {
            int64_t result = ::fseek( m_file, m_offset, SEEK_SET );

            if( result <= 0 )
            {
                LOGGER_ERROR( "invalid file '%s' seek offset %zu size %zu"
                    , fullPath
                    , m_offset
                    , m_size
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _fullPath )
    {
        if( Helper::concatenateFilePath( {_relationPath, _folderPath, _filePath}, _fullPath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s:%s'"
                , _relationPath.c_str()
                , _folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        FILE * file = ::fopen( _fullPath, "rb" );

        if( file == nullptr )
        {
            LOGGER_ERROR( "invalid open file '%s' error: %d"
                , _fullPath
                , errno
            );

            return false;
        }

        m_file = file;

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            const FilePath & folderPath = this->getDebugFolderPath();
            const FilePath & filePath = this->getDebugFilePath();

            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, folderPath.c_str(), filePath.c_str(), m_streaming );
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::close()
    {
        if( m_file == nullptr )
        {
            return true;
        }

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            const FilePath & folderPath = this->getDebugFolderPath();
            const FilePath & filePath = this->getDebugFilePath();

            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, folderPath.c_str(), filePath.c_str(), m_streaming );
        }
#endif

        ::fclose( m_file );
        m_file = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidFileInputStream::read( void * const _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileInputStream, this, "AndroidFileInputStream::read" );

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
    bool AndroidFileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        size_t bytesRead = ::fread( buf_offset, 1, _size, m_file );

        if( bytesRead == 0 )
        {
            *_read = 0;

            return true;
        }

        *_read = bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::seek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileInputStream, this, "AndroidFileInputStream::seek" );

        bool successful = this->seek_( _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidFileInputStream::rewind()
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileInputStream, this, "AndroidFileInputStream::rewind" );

        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::rseek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileInputStream, this, "AndroidFileInputStream::seek" );

        bool successful = this->seek_( m_size - _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::seek_( size_t _pos )
    {
        if( _pos >= m_reading - m_capacity && _pos <= m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
            int64_t seek_pos = m_offset + _pos;

            int64_t result = ::fseek( m_file, seek_pos, SEEK_SET );

            if( result < 0 )
            {
                LOGGER_ERROR( "file '%s:%s' error seek %zu:%zu"
                    , Helper::getDebugFolderPath( this ).c_str()
                    , Helper::getDebugFilePath( this ).c_str()
                    , _pos
                    , m_size
                );

                return false;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading = static_cast<size_t>(result) - m_offset;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::skip( size_t _size )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileInputStream, this, "AndroidFileInputStream::skip" );

        size_t current = m_reading - m_capacity + m_carriage;

        size_t seek_pos = current + _size;

        bool result = this->seek_( seek_pos );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidFileInputStream::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileInputStream, this, "AndroidFileInputStream::tell" );

        size_t current = m_reading - m_capacity + m_carriage;

        return current;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidFileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileInputStream, this, "AndroidFileInputStream::eof" );

        size_t current = m_reading - m_capacity + m_carriage;

        return current == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::time( uint64_t * const _time ) const
    {
        MENGINE_UNUSED( _time );

        int fd = ::fileno( m_file );

        struct stat fi;
        if( ::fstat( fd, &fi ) == -1 )
        {
            LOGGER_ERROR( "file '%s:%s' invalid get stat"
                , Helper::getDebugFolderPath( this ).c_str()
                , Helper::getDebugFilePath( this ).c_str()
            );

            return false;
        }

        *_time = (uint64_t)fi.st_mtime;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    FILE * AndroidFileInputStream::getFILE() const
    {
        return m_file;
    }
    //////////////////////////////////////////////////////////////////////////
}
