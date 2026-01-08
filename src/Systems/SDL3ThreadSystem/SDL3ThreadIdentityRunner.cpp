#include "SDL3ThreadIdentityRunner.h"

#include "Environment/SDL3/SDL3Includer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadIdentityRunner::SDL3ThreadIdentityRunner( const LambdaThreadRunner & _lambda, Timestamp _sleep )
        : m_lambda( _lambda )
        , m_sleep( _sleep )
        , m_cancel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadIdentityRunner::~SDL3ThreadIdentityRunner()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadIdentityRunner::run()
    {
        ThreadIdentityRunnerInterfacePtr runner = ThreadIdentityRunnerInterfacePtr::from( this );
        
        bool resume = m_lambda( runner );

        return resume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadIdentityRunner::cancel()
    {
        m_cancel = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadIdentityRunner::sleep()
    {
        if( m_sleep != 0 )
        {
            Uint32 sleep = (Uint32)m_sleep;

            SDL_Delay( sleep );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadIdentityRunner::isCancel() const
    {
        return m_cancel;
    }
    //////////////////////////////////////////////////////////////////////////
}