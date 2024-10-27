#include "Win32ThreadIdentityRunner.h"

#include "Environment/Windows/Win32Helper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentityRunner::Win32ThreadIdentityRunner( const LambdaThreadRunner & _lambda, uint32_t _sleep )
        : m_lambda( _lambda )
        , m_sleep( _sleep )
        , m_cancel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentityRunner::~Win32ThreadIdentityRunner()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentityRunner::run()
    {
        ThreadIdentityRunnerInterfacePtr runner = ThreadIdentityRunnerInterfacePtr::from( this );

        bool resume = m_lambda( runner );

        return resume;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentityRunner::sleep()
    {
        Helper::Win32Sleep( m_sleep );
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