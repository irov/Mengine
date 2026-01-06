#include "SDL3ThreadSharedMutex.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadSharedMutex::SDL3ThreadSharedMutex()
        : m_cs( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadSharedMutex::~SDL3ThreadSharedMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadSharedMutex::initialize()
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
    void SDL3ThreadSharedMutex::finalize()
    {
        if( m_cs != nullptr )
        {
            SDL_DestroyMutex( m_cs );
            m_cs = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadSharedMutex::lock()
    {
        SDL_LockMutex( m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadSharedMutex::unlock()
    {
        SDL_UnlockMutex( m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadSharedMutex::tryLock()
    {
        bool result = SDL_TryLockMutex( m_cs );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadSharedMutex::lockShared()
    {
        SDL_LockMutex( m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadSharedMutex::unlockShared()
    {
        SDL_UnlockMutex( m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadSharedMutex::tryLockShared()
    {
        bool result = SDL_TryLockMutex( m_cs );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadSharedMutex::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}



