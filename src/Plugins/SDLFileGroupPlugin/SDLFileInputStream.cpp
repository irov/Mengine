#include "SDLFileInputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"

#include "SDLFileHelper.h"

#include "stdex/memorycopy.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileInputStream::SDLFileInputStream()
        : m_rwops( nullptr )
        , m_size( 0 )
        , m_offset( 0 )
        , m_carriage( 0 )
        , m_capacity( 0 )
        , m_reading( 0 )
#ifdef MENGINE_DEBUG
        , m_streaming( false )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileInputStream::~SDLFileInputStream()
    {
        this->close_();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileInputStream::close_()
    {
#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, m_folderPath.c_str(), m_filePath.c_str(), m_streaming );
        }
#endif

        if( m_rwops != nullptr )
        {
            SDL_RWclose( m_rwops );
            m_rwops = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming )
    {
        STDEX_THREAD_GUARD_SCOPE( this, "SDLFileInputStream::open" );

#ifdef MENGINE_DEBUG
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;

        m_streaming = _streaming;
#endif

        Char fullPath[MENGINE_MAX_PATH];
        if( this->openFile_( _relationPath, _folderPath, _filePath, fullPath ) == false )
        {
            return false;
        }

        Sint64 size = SDL_RWsize( m_rwops );

        if( 0 > size )
        {
            this->close_();

            LOGGER_ERROR( "invalid file size '%s'"
                , fullPath
            );

            return false;
        }

        if( _offset + _size > size )
        {
            LOGGER_ERROR( "invalid file '%s' range %d:%d size %d"
                , fullPath
                , _offset
                , _size
                , size
            );

            return false;
        }

        m_size = _size == 0 ? (size_t)size : _size;
        m_offset = _offset;

        m_carriage = 0;
        m_capacity = 0;
        m_reading = 0;

        if( m_offset != 0 )
        {
            Sint64 result = SDL_RWseek( m_rwops, static_cast<Sint64>(m_offset), RW_SEEK_SET );

            if( 0 > result )
            {
                const char * sdlError = SDL_GetError();

                LOGGER_ERROR( "seek offset %d size %d get error %s"
                    , m_offset
                    , m_size
                    , sdlError
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * _fullPath )
    {
        if( Helper::concatenateFilePath( _relationPath, _folderPath, _filePath, _fullPath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invlalid concatenate filePath '%s':'%s'"
                , _folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        m_rwops = SDL_RWFromFile( _fullPath, "rb" );

        if( m_rwops == nullptr )
        {
            const char * sdl_error = SDL_GetError();

            LOGGER_ERROR( "invalid open '%s' error '%s'"
                , _fullPath
                , sdl_error
            );

            return false;
        }

#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, _folderPath.c_str(), _filePath.c_str(), m_streaming );
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLFileInputStream::read( void * _buf, size_t _count )
    {
        STDEX_THREAD_GUARD_SCOPE( this, "SDLFileInputStream::read" );

        size_t pos = m_reading - m_capacity + m_carriage;

        size_t correct_count = _count;

        if( pos + _count > m_size )
        {
            correct_count = m_size - pos;
        }

        if( correct_count == m_size )
        {
            size_t bytesRead;
            if( this->read_( _buf, correct_count, bytesRead ) == false )
            {
                return 0;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading += bytesRead;

            return bytesRead;
        }

        if( correct_count > MENGINE_FILE_STREAM_BUFFER_SIZE )
        {
            size_t tail = m_capacity - m_carriage;

            if( tail != 0 )
            {
                stdex::memorycopy( _buf, 0, m_readCache + m_carriage, tail );
            }

            size_t toRead = correct_count - tail;
            void * toBuffer = (uint8_t *)_buf + tail;

            size_t bytesRead;
            if( this->read_( toBuffer, toRead, bytesRead ) == false )
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
        if( this->read_( m_readCache, MENGINE_FILE_STREAM_BUFFER_SIZE, bytesRead ) == false )
        {
            return 0;
        }

        size_t readSize = (std::min)(correct_count - tail, bytesRead);

        stdex::memorycopy( _buf, tail, m_readCache, readSize );

        m_carriage = readSize;
        m_capacity = bytesRead;

        m_reading += bytesRead;

        return readSize + tail;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::read_( void * _buf, size_t _size, size_t & _read )
    {
        if( _size == 0 )
        {
            _read = 0;

            return true;
        }

        size_t bytesRead = SDL_RWread( m_rwops, _buf, 1, _size );

        if( bytesRead == 0 )
        {
            const char * sdl_error = SDL_GetError();

            LOGGER_ERROR( "read %d:%d get error '%s'"
                , _size
                , m_size
                , sdl_error
            );

            return false;
        }

        _read = bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::seek( size_t _pos )
    {
        STDEX_THREAD_GUARD_SCOPE( this, "SDLFileInputStream::seek" );

        bool successful = this->seek_( _pos );

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
            const Sint64 result = SDL_RWseek( m_rwops, static_cast<Sint64>(m_offset + _pos), RW_SEEK_SET );

            if( 0 > result )
            {
                const char * sdl_error = SDL_GetError();

                LOGGER_ERROR( "seek %d:%d get error %s"
                    , _pos
                    , m_size
                    , sdl_error
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
    bool SDLFileInputStream::skip( size_t _pos )
    {
        STDEX_THREAD_GUARD_SCOPE( this, "SDLFileInputStream::skip" );

        size_t current = m_reading - m_capacity + m_carriage;

        size_t seek_pos = current + _pos;

        bool result = this->seek_( seek_pos );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLFileInputStream::tell() const
    {
        STDEX_THREAD_GUARD_SCOPE( this, "SDLFileInputStream::tell" );

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
        STDEX_THREAD_GUARD_SCOPE( this, "SDLFileInputStream::eof" );

        return (m_reading - m_capacity + m_carriage) == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::time( uint64_t & _time ) const
    {
#ifdef MENGINE_DEBUG
        Char filePath[MENGINE_MAX_PATH];
        if( Helper::concatenateFilePath( m_relationPath, m_folderPath, m_filePath, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invlalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            return false;
        }

        uint64_t ft = PLATFORM_SERVICE()
            ->getFileTime( filePath );

        _time = ft;
        return true;
#else
        _time = 0;
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::memory( void ** _memory, size_t * _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
}
