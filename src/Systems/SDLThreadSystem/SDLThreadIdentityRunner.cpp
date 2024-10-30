#include "SDLThreadIdentityRunner.h"

#include "Environment/SDL/SDLIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadIdentityRunner::SDLThreadIdentityRunner( const LambdaThreadRunner & _lambda, Timestamp _sleep )
        : m_lambda( _lambda )
        , m_sleep( _sleep )
        , m_cancel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadIdentityRunner::~SDLThreadIdentityRunner()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentityRunner::run()
    {
        ThreadIdentityRunnerInterfacePtr runner = ThreadIdentityRunnerInterfacePtr::from( this );
        
        bool resume = m_lambda( runner );

        return resume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentityRunner::cancel()
    {
        m_cancel = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentityRunner::sleep()
    {
        if( m_sleep != 0 )
        {
            Uint32 sleep = (Uint32)m_sleep;

            SDL_Delay( sleep );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentityRunner::isCancel() const
    {
        return m_cancel;
    }
    //////////////////////////////////////////////////////////////////////////
}