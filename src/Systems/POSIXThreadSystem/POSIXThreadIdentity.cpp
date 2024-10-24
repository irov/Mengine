#include "POSIXThreadIdentity.h"

#include "Interface/AllocatorSystemInterface.h"
#include "Interface/PlatformSystemInterface.h"

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

            pthread_t current_threadId = ::pthread_self();
            
            const ConstString & name = thread->getName();
            
            const Char * name_str = name.c_str();
            
#if defined(MENGINE_PLATFORM_APPLE)
            ::pthread_setname_np( name_str );
#else
            ::pthread_setname_np( current_threadId, name_str );
#endif
            
            EThreadPriority priority = thread->getPriority();

            sched_param sch;
            int policy;
            ::pthread_getschedparam( current_threadId, &policy, &sch );

            int priority_min = ::sched_get_priority_min( policy );
            int priority_max = ::sched_get_priority_max( policy );

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

            if( ::pthread_setschedparam( current_threadId, policy, &sch ) != 0 )
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
    ThreadIdentityRunnerInterfacePtr POSIXThreadIdentity::run( const LambdaThreadRunner & _lambda, const DocumentInterfacePtr & _doc )
    {
        m_runner = Helper::makeFactorableUnique<POSIXThreadIdentityRunner>( _doc, _lambda );

        pthread_t threadId;
        int status = ::pthread_create( &threadId, nullptr, &Detail::s_treadJob, reinterpret_cast<void *>(this) );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid create thread error: %d"
                , status
            );

            return nullptr;
        }

        m_threadId = threadId;

        LOGGER_INFO( "thread", "create thread name: %s id: %" MENGINE_PRIu64 " priority: %d"
            , m_name.c_str()
            , (ThreadId)m_threadId
            , m_priority
        );

        return m_runner;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadIdentity::main()
    {
        ALLOCATOR_SYSTEM()
            ->beginThread( (ThreadId)m_threadId );

        PLATFORM_SYSTEM()
            ->beginThread( (ThreadId)m_threadId );

        MENGINE_PROFILER_THREAD( m_name.c_str() );
        
        ThreadIdentityRunnerInterfacePtr runner = m_runner;
        runner->run();

        PLATFORM_SYSTEM()
            ->endThread( (ThreadId)m_threadId );

        ALLOCATOR_SYSTEM()
            ->endThread( (ThreadId)m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId POSIXThreadIdentity::getThreadId() const
    {
        return (ThreadId)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadIdentity::join()
    {
        int status = ::pthread_join( m_threadId, nullptr );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid join thread: %" MENGINE_PRIu64 " error status: %d"
                , (ThreadId)m_threadId
                , status
            );
        }

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadIdentity::isCurrentThread() const
    {
        pthread_t current_threadId = ::pthread_self();

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
    const ConstString & POSIXThreadIdentity::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
}
