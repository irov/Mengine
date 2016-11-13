#	include "SDLFileInputStream.h"

#	include "Interface/UnicodeInterface.h"
#	include "Interface/NotificationServiceInterface.h"
#   include "Interface/SDLLayerInterface.h"

#   include "Utils/Logger/Logger.h"

#	include "stdex/memorycopy.h"

#   include "SDL_rwops.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileInputStream::SDLFileInputStream()
        : m_serviceProvider(nullptr)
        , m_rwops(nullptr)
        , m_size(0)
        , m_offset(0)
        , m_carriage(0)
        , m_capacity(0)
        , m_reading(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileInputStream::~SDLFileInputStream()
    {
        this->close_();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileInputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileInputStream::close_()
    {
        if( m_rwops != nullptr )
        {
            SDL_RWclose(m_rwops);
            m_rwops = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::open( const FilePath & _folder, const FilePath & _fileName, size_t _offset, size_t _size )
    {
        STDEX_THREAD_GUARD_SCOPE( this, "SDLFileInputStream::open" );

#	ifdef _DEBUG
        m_folder = _folder.c_str();
        m_fileName = _fileName.c_str();
#	endif

        Char filePath[MENGINE_MAX_PATH];
        if( this->openFile_( _folder, _fileName, filePath ) == false )
        {
            return false;
        }

        Sint64 size = SDL_RWsize(m_rwops);

        if( 0 > size )
        {
            this->close_();

            LOGGER_ERROR(m_serviceProvider)("SDLFileInputStream::open %s invalid file size"
                , filePath
                );

            return false;
        }

        if( _offset + _size > size )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileInputStream::open %s invalid file range %d:%d size %d"
                , filePath
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

                LOGGER_ERROR( m_serviceProvider )("Win32InputStream::open seek offset %d size %d get error %s"
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
    bool SDLFileInputStream::openFile_( const FilePath & _folder, const FilePath & _fileName, Char * _filePath )
    {
        if( SDLLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _fileName, _filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileInputStream::open invlalid concatenate filePath '%s':'%s'"
                , _folder.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        m_rwops = SDL_RWFromFile(_filePath, "rb");

        if ( m_rwops == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileInputStream::open %s invalid open"
                , _filePath
                );

            return false;
        }

#	ifdef _DEBUG
        if( SERVICE_EXIST( m_serviceProvider, NotificationServiceInterface ) == true )
        {
            NOTIFICATION_SERVICE( m_serviceProvider )
                ->notify( NOTIFICATOR_DEBUG_OPEN_FILE, _folder.c_str(), _fileName.c_str() );
        }
#	endif

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
        const size_t bytesRead = SDL_RWread(m_rwops, _buf, 1, _size);

        if( bytesRead == 0 )
        {
            const char* sdlError = SDL_GetError();

            LOGGER_ERROR(m_serviceProvider)("Win32InputStream::read %d:%d get error %s"
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

                LOGGER_ERROR(m_serviceProvider)("Win32InputStream::seek %d:%d get error %s"
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
        _time = 0;
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileInputStream::memory( void ** _memory, size_t * _size )
    {
        (void)_memory;
        (void)_size;

        return false;
    }
}	// namespace Menge
