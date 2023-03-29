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

#if defined(MENGINE_DEBUG)
        m_doc = _doc;
#endif

        SDL_mutex * cs = SDL_CreateMutex();

        if( cs == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error: %s (doc: %s)"
                , SDL_GetError()
                , MENGINE_DOCUMENT_STR( m_doc )
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

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadMutex::lock()
    {
        if( SDL_LockMutex( m_cs ) != 0 )
        {
            LOGGER_ERROR( "invalid lock error: %s (doc: %s)"
                , SDL_GetError()
                , MENGINE_DOCUMENT_STR( m_doc )
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
                , MENGINE_DOCUMENT_STR( m_doc )
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
            LOGGER_ERROR( "invalid try lock error: %s (doc: %s)"
                , SDL_GetError()
                , MENGINE_DOCUMENT_STR( m_doc )
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
