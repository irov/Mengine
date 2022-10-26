#include "SDLThreadIdentityRunner.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadIdentityRunner::SDLThreadIdentityRunner( const LambdaThreadRunner & _lambda )
        : m_lambda( _lambda )
        , m_process( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadIdentityRunner::~SDLThreadIdentityRunner()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentityRunner::run()
    {
        ThreadIdentityRunnerInterfacePtr runner = ThreadIdentityRunnerInterfacePtr::from( this );
        m_lambda( runner );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentityRunner::cancel()
    {
        m_process = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentityRunner::isCancel() const
    {
        return m_process;
    }
    //////////////////////////////////////////////////////////////////////////
}