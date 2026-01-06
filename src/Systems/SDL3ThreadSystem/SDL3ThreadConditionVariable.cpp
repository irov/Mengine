#include "SDL3ThreadConditionVariable.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadConditionVariable::SDL3ThreadConditionVariable()
        : m_conditionVariable( nullptr )
        , m_conditionLock( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadConditionVariable::~SDL3ThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadConditionVariable::initialize()
    {
        SDL_Mutex * conditionLock = SDL_CreateMutex();

        if( conditionLock == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error: %s"
                , SDL_GetError()
            );

            return false;
        }

        SDL_Condition * conditionVariable = SDL_CreateCondition();

        if( conditionVariable == nullptr )
        {
            LOGGER_ERROR( "invalid create condition error: %s"
                , SDL_GetError()
            );

            SDL_DestroyMutex( conditionLock );

            return false;
        }

        m_conditionLock = conditionLock;
        m_conditionVariable = conditionVariable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadConditionVariable::finalize()
    {
        if( m_conditionVariable != nullptr )
        {
            SDL_DestroyCondition( m_conditionVariable );
            m_conditionVariable = nullptr;
        }

        if( m_conditionLock != nullptr )
        {
            SDL_DestroyMutex( m_conditionLock );
            m_conditionLock = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadConditionVariable::wait( const LambdaCondition & _condition )
    {
        for( ;; )
        {
            SDL_LockMutex( m_conditionLock );

            bool result = SDL_WaitConditionTimeout( m_conditionVariable, m_conditionLock, 1000 );

            if( result == false )
            {
                // Timeout or error - continue waiting
            }

            SDL_UnlockMutex( m_conditionLock );

            if( _condition() == true )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadConditionVariable::wake()
    {
        SDL_SignalCondition( m_conditionVariable );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadConditionVariable::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}



