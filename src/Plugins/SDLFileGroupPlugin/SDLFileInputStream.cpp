#include "SDLFileInputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadGuardScope.h"

#include "SDLFileHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    SDLFileInputStream::SDLFileInputStream()
        : m_rwops( nullptr )
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
    SDLFileInputStream::~SDLFileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDLFileInputStream, this, "SDLFileInputStream::open" );

#ifdef MENGINE_DEBUG
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;
#endif

        m_streaming = _streaming;
        m_share = _share;

        Char fullPath[MENGINE_MAX_PATH] = {'\0'};
        if( this->openFile_( _relationPath, _folderPath, _filePath, fullPath ) == false )
        {
            return false;
        }

        Sint64 size = SDL_RWsize( m_rwops );

        if( 0 > size )
        {
            LOGGER_ERROR( "invalid file size '%s' [error: %s]"
                , fullPath
                , SDL_GetError()
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
            Sint64 result = SDL_RWseek( m_rwops, static_cast<Sint64>(m_offset), RW_SEEK_SET );

            if( 0 > result )
            {
                LOGGER_ERROR( "seek offset %zu size %zu get [error: %s]"
                    , m_offset
                    , m_size
                    , SDL_GetError()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _fullPath )
    {
        if( Helper::concatenateFilePath( _relationPath, _folderPath, _filePath, _fullPath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s':'%s'"
                , _folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        SDL_RWops * rwops = SDL_RWFromFile( _fullPath, "rb" );

        if( rwops == nullptr )
        {
            LOGGER_ERROR( "invalid open '%s' error '%s'"
                , _fullPath
                , SDL_GetError()
            );

            return false;
        }

        m_rwops = rwops;

#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, _folderPath.c_str(), _filePath.c_str(), m_streaming );
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::close()
    {
        if( m_rwops == nullptr )
        {
            return true;
        }

#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, m_folderPath.c_str(), m_filePath.c_str(), m_streaming );
        }
#endif

        int error = SDL_RWclose( m_rwops );
        m_rwops = nullptr;

        if( error != 0 )
        {
            LOGGER_ERROR( "invalid close '%s' get error: %s"
                , MENGINE_DEBUG_VALUE( m_filePath.c_str(), "" )
                , SDL_GetError()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLFileInputStream::read( void * const _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDLFileInputStream, this, "SDLFileInputStream::read" );

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
    bool SDLFileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        size_t bytesRead = SDL_RWread( m_rwops, buf_offset, 1, _size );

        if( bytesRead == 0 )
        {
            *_read = 0;

            return true;
        }

        *_read = bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::seek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDLFileInputStream, this, "SDLFileInputStream::seek" );

        bool successful = this->seek_( _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::rseek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDLFileInputStream, this, "SDLFileInputStream::seek" );

        bool successful = this->seek_( m_size - _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::seek_( size_t _pos )
    {
        if( _pos >= m_reading - m_capacity && _pos <= m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
            Sint64 result = SDL_RWseek( m_rwops, static_cast<Sint64>(m_offset + _pos), RW_SEEK_SET );

            if( 0 > result )
            {
                LOGGER_ERROR( "seek %zu:%zu get [error: %s]"
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
    bool SDLFileInputStream::skip( size_t _size )
    {
        MENGINE_THREAD_GUARD_SCOPE( SDLFileInputStream, this, "SDLFileInputStream::skip" );

        size_t current = m_reading - m_capacity + m_carriage;

        size_t seek_pos = current + _size;

        bool result = this->seek_( seek_pos );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLFileInputStream::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( SDLFileInputStream, this, "SDLFileInputStream::tell" );

        size_t current = m_reading - m_capacity + m_carriage;

        return current;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLFileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( SDLFileInputStream, this, "SDLFileInputStream::eof" );

        return (m_reading - m_capacity + m_carriage) == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::time( uint64_t * const _time ) const
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
    bool SDLFileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_RWops * SDLFileInputStream::getRWops() const
    {
        return m_rwops;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
    //////////////////////////////////////////////////////////////////////////
    const FilePath & SDLFileInputStream::getRelationPath() const
    {
        return m_relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & SDLFileInputStream::getFolderPath() const
    {
        return m_folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & SDLFileInputStream::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
}
