#include "SDLFileInputStream.h"

#include "Interface/UnicodeInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/SDLLayerInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"

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
        if( m_rwops != nullptr )
        {
            SDL_RWclose( m_rwops );
            m_rwops = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size )
    {
        STDEX_THREAD_GUARD_SCOPE( this, "SDLFileInputStream::open" );

#ifndef NDEBUG
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;
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

            LOGGER_ERROR("SDLFileInputStream::open %s invalid file size"
                , fullPath
                );

            return false;
        }

        if( _offset + _size > size )
        {
            LOGGER_ERROR("SDLFileInputStream::open %s invalid file range %d:%d size %d"
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
                const char* sdlError = SDL_GetError();

                LOGGER_ERROR("Win32InputStream::open seek offset %d size %d get error %s"
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
        if( SDLLAYER_SERVICE()
            ->concatenateFilePath( _relationPath, _folderPath, _filePath, _fullPath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "SDLFileInputStream::open invlalid concatenate filePath '%s':'%s'"
                , _folderPath.c_str()
                , _filePath.c_str()
                );

            return false;
        }

        m_rwops = SDL_RWFromFile( _fullPath, "rb" );

        if( m_rwops == nullptr )
        {
            LOGGER_ERROR( "SDLFileInputStream::open %s invalid open"
                , _fullPath
                );

            return false;
        }

#ifndef NDEBUG
        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_SERVICE()
                ->notify( NOTIFICATOR_DEBUG_OPEN_FILE, _folderPath.c_str(), _filePath.c_str() );
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
        const size_t bytesRead = SDL_RWread( m_rwops, _buf, 1, _size );

        if( bytesRead == 0 )
        {
            const char* sdlError = SDL_GetError();

            LOGGER_ERROR( "Win32InputStream::read %d:%d get error %s"
                , _size
                , m_size
                , sdlError
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
                const char* sdlError = SDL_GetError();

                LOGGER_ERROR( "Win32InputStream::seek %d:%d get error %s"
                    , _pos
                    , m_size
                    , sdlError
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
#ifndef NDEBUG
        Char filePath[MENGINE_MAX_PATH];
        if( SDLLAYER_SERVICE()
            ->concatenateFilePath( m_relationPath, m_folderPath, m_filePath, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "SDLFileInputStream::open invlalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
                );

            return false;
        }

        WString unicode_filePath;
        Helper::utf8ToUnicodeSize( filePath, UNICODE_UNSIZE, unicode_filePath );

        uint64_t ft = PLATFORM_SERVICE()
            ->getFileTime( unicode_filePath );

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
        (void)_memory;
        (void)_size;

        return false;
    }
}	// namespace Mengine
