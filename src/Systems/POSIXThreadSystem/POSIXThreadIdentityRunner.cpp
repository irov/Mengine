#include "POSIXThreadIdentityRunner.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadIdentityRunner::POSIXThreadIdentityRunner( const LambdaThreadRunner & _lambda )
        : m_lambda( _lambda )
        , m_cancel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadIdentityRunner::~POSIXThreadIdentityRunner()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadIdentityRunner::run()
    {
        ThreadIdentityRunnerInterfacePtr runner = ThreadIdentityRunnerInterfacePtr::from( this );

        m_lambda( runner );
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadIdentityRunner::cancel()
    {
        m_cancel = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadIdentityRunner::isCancel() const
    {
        return m_cancel;
    }
    //////////////////////////////////////////////////////////////////////////
}