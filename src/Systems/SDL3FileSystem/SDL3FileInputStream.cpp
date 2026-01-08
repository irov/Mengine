#include "SDL3FileInputStream.h"

#include "Interface/FileSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/DebugFileHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    SDL3FileInputStream::SDL3FileInputStream()
        : m_IOStream( nullptr )
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
    SDL3FileInputStream::~SDL3FileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDL3FileInputStream, this );

        m_streaming = _streaming;
        m_share = _share;

        Path fullPath = {'\0'};
        if( Helper::concatenateFilePath( {_relationPath, _folderPath, _filePath}, fullPath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                , _folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        SDL_IOStream * iostream = SDL_IOFromFile( fullPath, "rb" );

        if( iostream == nullptr )
        {
            LOGGER_ERROR( "invalid open file '%s' error '%s'"
                , fullPath
                , SDL_GetError()
            );

            return false;
        }

        m_IOStream = iostream;

        Sint64 size = SDL_GetIOSize( m_IOStream );

        if( 0 > size )
        {
            LOGGER_ERROR( "invalid file '%s' size %zu [error: %s]"
                , fullPath
                , (size_t)size
                , SDL_GetError()
            );

            this->close();

            return false;
        }

        if( _size != MENGINE_UNKNOWN_SIZE )
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
            Sint64 result = SDL_SeekIO( m_IOStream, static_cast<Sint64>(m_offset), SDL_IO_SEEK_SET );

            if( 0 > result )
            {
                LOGGER_ERROR( "file '%s' seek offset %zu size %zu get [error: %s]"
                    , fullPath
                    , m_offset
                    , m_size
                    , SDL_GetError()
                );

                return false;
            }
        }

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, _folderPath, _filePath, true, m_streaming );
        }
#endif

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::addDebugFilePath( this, _relationPath, _folderPath, _filePath, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileInputStream::close()
    {
        if( m_IOStream == nullptr )
        {
            return;
        }

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            const FilePath & folderPath = Helper::getDebugFolderPath( this );
            const FilePath & filePath = Helper::getDebugFilePath( this );

            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, folderPath, filePath, true, m_streaming );
        }
#endif

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif

        bool error = SDL_CloseIO( m_IOStream );

        m_IOStream = nullptr;

        m_size = 0;

        if( error == false )
        {
            LOGGER_ERROR( "invalid close file '%s' get error: %s"
                , Helper::getDebugFullPath( this ).c_str()
                , SDL_GetError()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL3FileInputStream::read( void * const _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDL3FileInputStream, this );

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
    bool SDL3FileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        size_t bytesRead = SDL_ReadIO( m_IOStream, buf_offset, _size );

        if( bytesRead == 0 )
        {
            *_read = 0;

            return true;
        }

        *_read = bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileInputStream::seek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDL3FileInputStream, this );

        bool successful = this->seek_( _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileInputStream::rewind()
    {
        MENGINE_THREAD_GUARD_SCOPE( SDL3FileInputStream, this );

        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileInputStream::rseek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDL3FileInputStream, this );

        bool successful = this->seek_( m_size - _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileInputStream::seek_( size_t _pos )
    {
        if( _pos >= m_reading - m_capacity && _pos <= m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
            Sint64 seek_pos = static_cast<Sint64>(m_offset + _pos);

            Sint64 result = SDL_SeekIO( m_IOStream, seek_pos, SDL_IO_SEEK_SET );

            if( result < 0 )
            {
                LOGGER_ERROR( "file '%s' seek %zu:%zu get [error: %s]"
                    , Helper::getDebugFullPath( this ).c_str()
                    , _pos
                    , m_size
                    , SDL_GetError()
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
    bool SDL3FileInputStream::skip( size_t _size )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDL3FileInputStream, this );

        size_t current = m_reading - m_capacity + m_carriage;

        size_t seek_pos = current + _size;

        bool result = this->seek_( seek_pos );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL3FileInputStream::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( SDL3FileInputStream, this );

        size_t current = m_reading - m_capacity + m_carriage;

        return current;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL3FileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileInputStream::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( SDL3FileInputStream, this );

        return (m_reading - m_capacity + m_carriage) == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileInputStream::time( uint64_t * const _time ) const
    {
#if defined(MENGINE_DEBUG)
        const FilePath & relationPath = Helper::getDebugRelationPath( this );
        const FilePath & folderPath = Helper::getDebugFolderPath( this );
        const FilePath & filePath = Helper::getDebugFilePath( this );

        Path fullPath = {'\0'};
        if( Helper::concatenateFilePath( {relationPath, folderPath, filePath}, fullPath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s:%s'"
                , relationPath.c_str()
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
    bool SDL3FileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_IOStream * SDL3FileInputStream::getIOStream() const
    {
        return m_IOStream;
    }    
    //////////////////////////////////////////////////////////////////////////
}