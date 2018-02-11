#   include "SDLThreadMutex.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadMutex::SDLThreadMutex()
        : m_cs( nullptr )
        , m_file( nullptr )
		, m_line( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadMutex::initialize( const char * _file, uint32_t _line )
    {
        m_file = _file;
		m_line = _line;

        SDL_mutex * cs = SDL_CreateMutex();

        if( cs == nullptr )
        {
            return false;
        }

        m_cs = cs;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::lock()
    {
        const int err = SDL_LockMutex( m_cs );

        if( err != 0 )
        {
            LOGGER_ERROR("SDLThreadMutex::lock invalid lock"
                );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::unlock()
    {
        const int err = SDL_UnlockMutex( m_cs );

        if( err != 0 )
        {
            LOGGER_ERROR("SDLThreadMutex::unlock invalid unlock"
                );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadMutex::try_lock()
    {
        const int err = SDL_TryLockMutex( m_cs );

        if( err == 0 )
        {
            return true;
        }
        else if( err == -1 )
        {
            const char * err_msg = SDL_GetError();
            
            LOGGER_ERROR("SDLThreadMutex::lock invalid try lock (msg %s)"
                , err_msg
                );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::_destroy()
    {
        SDL_DestroyMutex( m_cs );
        m_cs = nullptr;
    }
}
