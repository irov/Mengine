#include "POSIXThreadIdentityRunner.h"

#include <unistd.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadIdentityRunner::POSIXThreadIdentityRunner( const LambdaThreadRunner & _lambda, uint32_t _sleep )
        : m_lambda( _lambda )
        , m_sleep( _sleep )
        , m_cancel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadIdentityRunner::~POSIXThreadIdentityRunner()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadIdentityRunner::run()
    {
        ThreadIdentityRunnerInterfacePtr runner = ThreadIdentityRunnerInterfacePtr::from( this );

        bool resume = m_lambda( runner );

        return resume;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadIdentityRunner::sleep()
    {
        ::usleep( m_sleep * 1000 );
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