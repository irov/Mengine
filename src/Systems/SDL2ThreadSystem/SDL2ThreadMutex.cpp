#include "SDL2ThreadMutex.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2ThreadMutex::SDL2ThreadMutex()
        : m_cs( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2ThreadMutex::~SDL2ThreadMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2ThreadMutex::initialize()
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
    void SDL2ThreadMutex::finalize()
    {
        if( m_cs != nullptr )
        {
            SDL_DestroyMutex( m_cs );
            m_cs = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadMutex::lock()
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
    void SDL2ThreadMutex::unlock()
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
    bool SDL2ThreadMutex::tryLock()
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
    void SDL2ThreadMutex::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
