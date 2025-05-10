#include "SDL2ThreadIdentityRunner.h"

#include "Environment/SDL2/SDL2Includer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2ThreadIdentityRunner::SDL2ThreadIdentityRunner( const LambdaThreadRunner & _lambda, Timestamp _sleep )
        : m_lambda( _lambda )
        , m_sleep( _sleep )
        , m_cancel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2ThreadIdentityRunner::~SDL2ThreadIdentityRunner()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2ThreadIdentityRunner::run()
    {
        ThreadIdentityRunnerInterfacePtr runner = ThreadIdentityRunnerInterfacePtr::from( this );
        
        bool resume = m_lambda( runner );

        return resume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadIdentityRunner::cancel()
    {
        m_cancel = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadIdentityRunner::sleep()
    {
        if( m_sleep != 0 )
        {
            Uint32 sleep = (Uint32)m_sleep;

            SDL_Delay( sleep );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2ThreadIdentityRunner::isCancel() const
    {
        return m_cancel;
    }
    //////////////////////////////////////////////////////////////////////////
}