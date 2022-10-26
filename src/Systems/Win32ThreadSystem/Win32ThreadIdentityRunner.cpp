#include "Win32ThreadIdentityRunner.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentityRunner::Win32ThreadIdentityRunner( const LambdaThreadRunner & _lambda )
        : m_lambda( _lambda )
        , m_cancel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentityRunner::~Win32ThreadIdentityRunner()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentityRunner::run()
    {
        ThreadIdentityRunnerInterfacePtr runner = ThreadIdentityRunnerInterfacePtr::from( this );
        m_lambda( runner );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentityRunner::cancel()
    {
        m_cancel = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentityRunner::isCancel() const
    {
        return m_cancel;
    }
    //////////////////////////////////////////////////////////////////////////
}