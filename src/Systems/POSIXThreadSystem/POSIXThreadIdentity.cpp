#include "POSIXThreadIdentity.h"

#include "Interface/AllocatorSystemInterface.h"

#include "POSIXThreadIdentityRunner.h"

#include "Kernel/Logger.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ThreadEnum.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"

#include <unistd.h>
#include <errno.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * s_treadJob( void * _userData )
        {
            POSIXThreadIdentity * thread = reinterpret_cast<POSIXThreadIdentity *>(_userData);

            EThreadPriority priority = thread->getPriority();

            pthread_t current_threadId = pthread_self();

            sched_param sch;
            int policy;
            pthread_getschedparam( current_threadId, &policy, &sch );

            int priority_min = sched_get_priority_min( policy );
            int priority_max = sched_get_priority_max( policy );

            switch( priority )
            {
            case ETP_LOWEST:
                sch.sched_priority = priority_min;
                break;
            case ETP_BELOW_NORMAL:
                sch.sched_priority = priority_min + (priority_max - priority_min) * 1 / 3;
                break;
            case ETP_NORMAL:
                sch.sched_priority = priority_min + (priority_max - priority_min) / 2;
                break;
            case ETP_ABOVE_NORMAL:
                sch.sched_priority = priority_min + (priority_max - priority_min) * 2 / 3;
                break;
            case ETP_HIGHEST:
                sch.sched_priority = priority_min + (priority_max - priority_min) * 3 / 4;
                break;
            case ETP_TIME_CRITICAL:
                sch.sched_priority = priority_max;
                break;
            default:
                break;
            }

            if( pthread_setschedparam( current_threadId, policy, &sch ) != 0 )
            {
                LOGGER_ERROR( "Failed to set thread priority: %d"
                    , sch.sched_priority 
                );
            }

            thread->main();

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadIdentity::POSIXThreadIdentity()
        : m_priority( ETP_NORMAL )
        , m_threadId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadIdentity::~POSIXThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadIdentity::initialize( EThreadPriority _priority, const ConstString & _name )
    {
        m_priority = _priority;
        m_name = _name;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadIdentity::finalize()
    {
        m_threadId = 0;
        m_runner = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityRunnerInterfacePtr POSIXThreadIdentity::run( const LambdaThreadRunner & _lambda )
    {
        m_runner = Helper::makeFactorableUnique<POSIXThreadIdentityRunner>( MENGINE_DOCUMENT_FACTORABLE, _lambda );

        pthread_t threadId;
        int status = pthread_create( &threadId, nullptr, &Detail::s_treadJob, reinterpret_cast<void *>(this) );

        if( thread == nullptr )
        {
            LOGGER_ERROR( "invalid create thread error: %d"
                , status
            );

            return nullptr;
        }

        m_threadId = threadId;        

        return m_runner;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadIdentity::main()
    {
        ALLOCATOR_SYSTEM()
            ->beginThread( m_threadId );

        MENGINE_PROFILER_THREAD( m_name.c_str() );
        
        ThreadIdentityRunnerInterfacePtr runner = m_runner;
        runner->run();

        ALLOCATOR_SYSTEM()
            ->endThread( m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId POSIXThreadIdentity::getThreadId() const
    {
        return (ThreadId)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadIdentity::join()
    {
        if( m_runner->isCancel() == true )
        {
            return;
        }

        int status = pthread_join( m_threadId, nullptr );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid join thread: %d error status: %d"
                , m_threadId
                , status
            );
        }

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadIdentity::cancel()
    {
        if( m_runner->isCancel() == true )
        {
            return;
        }

        m_runner->cancel();

        int status = pthread_cancel( m_threadId );
        
        if( status != 0 )
        {
            LOGGER_ERROR( "invalid cancel thread: %s error status: %d"
                , m_threadId
                , status
            );
        }

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadIdentity::isCurrentThread() const
    {
        pthread_t current_threadId = pthread_self();

        if( m_threadId != current_threadId )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EThreadPriority POSIXThreadIdentity::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
}
