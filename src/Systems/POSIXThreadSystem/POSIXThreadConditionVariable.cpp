#include "POSIXThreadConditionVariable.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadConditionVariable::POSIXThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadConditionVariable::~POSIXThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadConditionVariable::initialize()
    {
        int condition_status = ::pthread_cond_init( &m_conditionVariable, nullptr );

        if( condition_status != 0 )
        {
            LOGGER_ERROR( "Failed to initialize condition variable: %d"
                , condition_status
            );

            return false;
        }

        int mutex_status = ::pthread_mutex_init( &m_conditionLock, nullptr );

        if( mutex_status != 0 )
        {
            LOGGER_ERROR( "Failed to initialize mutex: %d"
                , mutex_status 
            );

            ::pthread_cond_destroy( &m_conditionVariable );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadConditionVariable::finalize()
    {
        ::pthread_cond_destroy( &m_conditionVariable );
        ::pthread_mutex_destroy( &m_conditionLock );
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadConditionVariable::wait( const LambdaCondition & _condition )
    {
        for( ;; )
        {
            ::pthread_mutex_lock( &m_conditionLock );
            
            struct timespec ts;
            ::clock_gettime(CLOCK_REALTIME, &ts);
            
            ts.tv_sec += 1;
            
            ::pthread_cond_timedwait( &m_conditionVariable, &m_conditionLock, &ts );
            
            ::pthread_mutex_unlock( &m_conditionLock );

            if( _condition() == true )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadConditionVariable::wake()
    {
        int status = ::pthread_cond_signal( &m_conditionVariable );
        
        if( status != 0 )
        {
            LOGGER_ERROR( "invalid cond signal error: %d"
                , status
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadConditionVariable::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
