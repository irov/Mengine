#include "SDLThreadSharedMutex.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadSharedMutex::SDLThreadSharedMutex()
        : m_cs( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadSharedMutex::~SDLThreadSharedMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadSharedMutex::initialize()
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
    void SDLThreadSharedMutex::finalize()
    {
        if( m_cs != nullptr )
        {
            SDL_DestroyMutex( m_cs );
            m_cs = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadSharedMutex::lock()
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
    void SDLThreadSharedMutex::unlock()
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
    bool SDLThreadSharedMutex::tryLock()
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
    void SDLThreadSharedMutex::lockShared()
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
    void SDLThreadSharedMutex::unlockShared()
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
    bool SDLThreadSharedMutex::tryLockShared()
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
    void SDLThreadSharedMutex::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
