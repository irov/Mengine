#include "POSIXThreadIdentityRunner.h"

#include <unistd.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadIdentityRunner::POSIXThreadIdentityRunner( const LambdaThreadRunner & _lambda, Timestamp _sleep )
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
        useconds_t sleep = (useconds_t)m_sleep * 1000;

        ::usleep( sleep );
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