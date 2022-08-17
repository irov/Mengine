#include "SDLThreadMutex.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadMutex::SDLThreadMutex()
        : m_cs( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadMutex::~SDLThreadMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadMutex::initialize( const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

#ifdef MENGINE_DEBUG
        m_doc = _doc;
#endif

        SDL_mutex * cs = SDL_CreateMutex();

        if( cs == nullptr )
        {
            const char * err_str = SDL_GetError();

            LOGGER_ERROR( "invalid create mutex error: %s"
                , err_str
            );

            return false;
        }

        m_cs = cs;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::lock()
    {
        if( SDL_LockMutex( m_cs ) != 0 )
        {
            const char * err_str = SDL_GetError();

            LOGGER_ERROR( "invalid lock error: %s"
                , err_str
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::unlock()
    {
        if( SDL_UnlockMutex( m_cs ) != 0 )
        {
            const char * err_str = SDL_GetError();

            LOGGER_ERROR( "invalid unlock error: %s"
                , err_str
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadMutex::try_lock()
    {
        int err = SDL_TryLockMutex( m_cs );

        if( err == 0 )
        {
            return true;
        }
        else if( err == -1 )
        {
            const char * err_msg = SDL_GetError();

            LOGGER_ERROR( "invalid try lock error: %s"
                , err_msg
            );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::_destroy()
    {
        if( m_cs != nullptr )
        {
            SDL_DestroyMutex( m_cs );
            m_cs = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
