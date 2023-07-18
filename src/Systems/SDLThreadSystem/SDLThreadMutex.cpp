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
    bool SDLThreadMutex::initialize()
    {
        SDL_mutex * cs = SDL_CreateMutex();

        if( cs == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error: %s (doc: %s)"
                , SDL_GetError()
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );

            return false;
        }

        m_cs = cs;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::finalize()
    {
        if( m_cs != nullptr )
        {
            SDL_DestroyMutex( m_cs );
            m_cs = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::lock()
    {
        if( SDL_LockMutex( m_cs ) != 0 )
        {
            LOGGER_ERROR( "invalid lock error: %s (doc: %s)"
                , SDL_GetError()
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::unlock()
    {
        if( SDL_UnlockMutex( m_cs ) != 0 )
        {
            LOGGER_ERROR( "invalid unlock error: %s (doc: %s)"
                , SDL_GetError()
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadMutex::tryLock()
    {
        int err = SDL_TryLockMutex( m_cs );

        if( err == 0 )
        {
            return true;
        }
        else if( err == -1 )
        {
            LOGGER_ERROR( "invalid try lock error: %s (doc: %s)"
                , SDL_GetError()
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
