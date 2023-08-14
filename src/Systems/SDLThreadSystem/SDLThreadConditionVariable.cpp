#include "SDLThreadConditionVariable.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadConditionVariable::SDLThreadConditionVariable()
        : m_conditionVariable( nullptr )
        , m_conditionLock( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadConditionVariable::~SDLThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadConditionVariable::initialize()
    {
        SDL_mutex * conditionLock = SDL_CreateMutex();

        if( conditionLock == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error: %s"
                , SDL_GetError()
            );

            return false;
        }

        SDL_cond * conditionVariable = SDL_CreateCond();

        if( conditionVariable == nullptr )
        {
            LOGGER_ERROR( "invalid create condition error: %s"
                , SDL_GetError()
            );

            return false;
        }

        m_conditionLock = conditionLock;
        m_conditionVariable = conditionVariable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadConditionVariable::finalize()
    {
        if( m_conditionVariable != nullptr )
        {
            SDL_DestroyCond( m_conditionVariable );
            m_conditionVariable = nullptr;
        }

        if( m_conditionLock != nullptr )
        {
            SDL_DestroyMutex( m_conditionLock );
            m_conditionLock = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadConditionVariable::wait( const LambdaCondition & _condition )
    {
        for( ;; )
        {
            if( SDL_LockMutex( m_conditionLock ) != 0 )
            {
                LOGGER_ERROR( "invalid lock mutex error: %s"
                    , SDL_GetError()
                );
            }

            int err = SDL_CondWaitTimeout( m_conditionVariable, m_conditionLock, 1000 );

            if( err == -1 )
            {
                LOGGER_ERROR( "invalid cond wait timeout error: %s"
                    , SDL_GetError()
                );
            }

            if( SDL_UnlockMutex( m_conditionLock ) != 0 )
            {
                LOGGER_ERROR( "invalid unlock mutex error: %s"
                    , SDL_GetError()
                );
            }

            if( _condition() == true )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadConditionVariable::wake()
    {
        if( SDL_CondSignal( m_conditionVariable ) != 0 )
        {
            LOGGER_ERROR( "invalid cond signal error: %s"
                , SDL_GetError()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadConditionVariable::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
