#include "SDL2ThreadSharedMutex.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2ThreadSharedMutex::SDL2ThreadSharedMutex()
        : m_cs( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2ThreadSharedMutex::~SDL2ThreadSharedMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2ThreadSharedMutex::initialize()
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
    void SDL2ThreadSharedMutex::finalize()
    {
        if( m_cs != nullptr )
        {
            SDL_DestroyMutex( m_cs );
            m_cs = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadSharedMutex::lock()
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
    void SDL2ThreadSharedMutex::unlock()
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
    bool SDL2ThreadSharedMutex::tryLock()
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
    void SDL2ThreadSharedMutex::lockShared()
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
    void SDL2ThreadSharedMutex::unlockShared()
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
    bool SDL2ThreadSharedMutex::tryLockShared()
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
    void SDL2ThreadSharedMutex::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
