#include "POSIXThreadProcessor.h"

#include "Interface/AllocatorSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadEnum.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * threadJob( void * _userData )
        {
            POSIXThreadProcessor * thread = reinterpret_cast<POSIXThreadProcessor *>(_userData);

            EThreadPriority priority = thread->getPriority();

            pthread_t current_threadId = ::pthread_self();

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
    POSIXThreadProcessor::POSIXThreadProcessor()
        : m_priority( ETP_NORMAL )
        , m_threadId( 0 )
        , m_task( nullptr )
        , m_exit( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadProcessor::~POSIXThreadProcessor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadProcessor::initialize( EThreadPriority _priority, const ConstString & _name, const ThreadMutexInterfacePtr & _mutex )
    {
        m_priority = _priority;
        m_name = _name;

        m_mutex = _mutex;

        ::pthread_mutex_init( &m_taskLock, nullptr );
        ::pthread_mutex_init( &m_processLock, nullptr );
        ::pthread_mutex_init( &m_conditionLock, nullptr );
        ::pthread_cond_init( &m_conditionVariable, nullptr );

        pthread_t threadId;
        int status = ::pthread_create( &threadId, nullptr, &Detail::threadJob, reinterpret_cast<void *>(this) );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid create thread error: %d"
                , status
            );

            return false;
        }

        ::pthread_setname_np( threadId, m_name.c_str() );

        m_threadId = threadId;

        LOGGER_INFO( "thread", "create thread name: %s id: %ld priority: %d"
            , m_name.c_str()
            , m_threadId
            , m_priority
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadProcessor::finalize()
    {
        ::pthread_mutex_destroy( &m_taskLock );
        ::pthread_mutex_destroy( &m_processLock );
        ::pthread_mutex_destroy( &m_conditionLock );
        ::pthread_cond_destroy( &m_conditionVariable );

        m_threadId = 0;
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadProcessor::main()
    {
        ALLOCATOR_SYSTEM()
            ->beginThread( m_threadId );

        while( m_exit == false )
        {
            ::pthread_mutex_lock( &m_conditionLock );
            ::pthread_cond_wait( &m_conditionVariable, &m_conditionLock );
            ::pthread_mutex_unlock( &m_conditionLock );

            if( m_exit == true )
            {
                break;
            }

            ::pthread_mutex_lock( &m_processLock );

            if( m_task != nullptr )
            {
                if( m_exit == false )
                {
                    m_mutex->lock();
                    m_task->main();
                    m_mutex->unlock();
                }

                ::pthread_mutex_lock( &m_taskLock );

                m_task = nullptr;

                ::pthread_mutex_unlock( &m_taskLock );
            }

            ::pthread_mutex_unlock( &m_processLock );
        }

        ALLOCATOR_SYSTEM()
            ->endThread( m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId POSIXThreadProcessor::getThreadId() const
    {
        return (ThreadId)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadProcessor::processTask( ThreadTaskInterface * _task )
    {
        if( m_exit == true )
        {
            return false;
        }

        int processLockResult = ::pthread_mutex_trylock( &m_processLock );

        if( processLockResult != 0 )
        {
            return false;
        }

        bool successful = false;

        if( m_task == nullptr )
        {
            if( _task->run( m_mutex ) == true )
            {
                ::pthread_mutex_lock( &m_taskLock );

                m_task = _task;

                ::pthread_mutex_unlock( &m_taskLock );

                ::pthread_cond_signal( &m_conditionVariable );

                successful = true;
            }
        }

        ::pthread_mutex_unlock( &m_processLock );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadProcessor::removeTask()
    {
        if( m_exit == true )
        {
            return;
        }

        ::pthread_mutex_lock( &m_taskLock );

        if( m_task != nullptr )
        {
            m_task->cancel();
        }

        ::pthread_mutex_unlock( &m_taskLock );

        ::pthread_mutex_lock( &m_processLock );

        m_task = nullptr;

        ::pthread_mutex_unlock( &m_processLock );
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadProcessor::join()
    {
        if( m_exit == true )
        {
            return;
        }

        m_exit = true;

        ::pthread_cond_signal( &m_conditionVariable );

        void * data;
        int status = ::pthread_join( m_threadId, &data );

        if( status != 0 )
        {
            LOGGER_ERROR( "join thread: %ld with error status: %d"
                , m_threadId
                , status
            );
        }

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadProcessor::isCurrentThread() const
    {
        pthread_t current_threadId = ::pthread_self();

        if( m_threadId != current_threadId )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EThreadPriority POSIXThreadProcessor::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
}
