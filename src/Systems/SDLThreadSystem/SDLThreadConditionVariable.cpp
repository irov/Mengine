#include "SDLThreadConditionVariable.h"

#include "Kernel/Logger.h"
#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadConditionVariable::SDLThreadConditionVariable()
#ifndef NDEBUG
        : m_file( nullptr )
        , m_line( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadConditionVariable::~SDLThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadConditionVariable::initialize( const Char * _file, uint32_t _line )
    {
        MENGINE_UNUSED( _file );
        MENGINE_UNUSED( _line );

#ifndef NDEBUG
        m_file = _file;
        m_line = _line;
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