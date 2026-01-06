#include "SDL3ThreadMutex.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadMutex::SDL3ThreadMutex()
        : m_cs( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadMutex::~SDL3ThreadMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadMutex::initialize()
    {
        SDL_Mutex * cs = SDL_CreateMutex();

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
    void SDL3ThreadMutex::finalize()
    {
        if( m_cs != nullptr )
        {
            SDL_DestroyMutex( m_cs );
            m_cs = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadMutex::lock()
    {
        SDL_LockMutex( m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadMutex::unlock()
    {
        SDL_UnlockMutex( m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadMutex::tryLock()
    {
        bool result = SDL_TryLockMutex( m_cs );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadMutex::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}



