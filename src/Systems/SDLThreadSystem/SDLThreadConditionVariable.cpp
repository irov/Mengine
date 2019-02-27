#include "SDLThreadConditionVariable.h"

#include "Kernel/Logger.h"
#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadConditionVariable::SDLThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadConditionVariable::~SDLThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadConditionVariable::initialize( const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#ifdef MENGINE_DEBUG
        m_doc = _doc;
#endif

        SDL_mutex * conditionLock = SDL_CreateMutex();

        if( conditionLock == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error code - '%s'"
                , SDL_GetError()
            );

            return false;
        }

        SDL_cond * conditionVariable = SDL_CreateCond();

        if( conditionVariable == nullptr )
        {
            LOGGER_ERROR( "invalid create condition error code - %s"
                , SDL_GetError()
            );

            return false;
        }

        m_conditionLock = conditionLock;
        m_conditionVariable = conditionVariable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadConditionVariable::wait()
    {
        SDL_CondWait( m_conditionVariable, m_conditionLock );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadConditionVariable::wake()
    {
        SDL_CondSignal( m_conditionVariable );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadConditionVariable::_destroy()
    {
        SDL_CondSignal( m_conditionVariable );
        SDL_DestroyCond( m_conditionVariable );
        SDL_DestroyMutex( m_conditionLock );
    }
}