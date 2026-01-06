#include "SDL3ThreadProcessor.h"

#include "Interface/AllocatorSystemInterface.h"
#include "Interface/PlatformSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadEnum.h"
#include "Kernel/ProfilerHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int treadJob( void * _userData )
        {
            SDL3ThreadProcessor * thread = reinterpret_cast<SDL3ThreadProcessor *>(_userData);

            EThreadPriority priority = thread->getPriority();

            switch( priority )
            {
            case ETP_LOWEST:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_LOW );
                }break;
            case ETP_BELOW_NORMAL:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_LOW );
                }break;
            case ETP_NORMAL:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
                }break;
            case ETP_ABOVE_NORMAL:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
                }break;
            case ETP_HIGHEST:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_HIGH );
                }break;
            case ETP_TIME_CRITICAL:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_TIME_CRITICAL );
                }break;
            default:
                break;
            }

            thread->main();

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadProcessor::SDL3ThreadProcessor()
        : m_priority( ETP_NORMAL )
        , m_threadId( 0 )
        , m_thread( nullptr )
        , m_taskLock( nullptr )
        , m_processLock( nullptr )
        , m_conditionVariable( nullptr )
        , m_conditionLock( nullptr )
        , m_task( nullptr )
        , m_exit( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadProcessor::~SDL3ThreadProcessor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadProcessor::initialize( const ThreadDescription & _description, EThreadPriority _priority, const ThreadMutexInterfacePtr & _mutex )
    {
        m_priority = _priority;
        m_description = _description;

        m_mutex = _mutex;

        SDL_Mutex * taskLock = SDL_CreateMutex();

        if( taskLock == nullptr )
        {
            LOGGER_ERROR( "invalid create thread: %s mutex error: %s"
                , m_description.nameA
                , SDL_GetError()
            );

            return false;
        }

        m_taskLock = taskLock;

        SDL_Mutex * processLock = SDL_CreateMutex();

        if( processLock == nullptr )
        {
            LOGGER_ERROR( "invalid create thread: %s mutex error: %s"
                , m_description.nameA
                , SDL_GetError()
            );

            return false;
        }

        m_processLock = processLock;

        SDL_Mutex * conditionLock = SDL_CreateMutex();

        if( conditionLock == nullptr )
        {
            LOGGER_ERROR( "invalid create thread: %s mutex error: %s"
                , m_description.nameA
                , SDL_GetError()
            );

            return false;
        }

        SDL_Condition * conditionVariable = SDL_CreateCondition();

        if( conditionVariable == nullptr )
        {
            LOGGER_ERROR( "invalid create thread: %s condition error: %s"
                , m_description.nameA
                , SDL_GetError()
            );

            return false;
        }

        m_conditionLock = conditionLock;
        m_conditionVariable = conditionVariable;

        SDL_Thread * thread = SDL_CreateThread( &Detail::treadJob, m_description.nameA, reinterpret_cast<void *>(this) );

        if( thread == nullptr )
        {
            LOGGER_ERROR( "invalid create thread: %s error: %s"
                , m_description.nameA
                , SDL_GetError()
            );

            return false;
        }

        m_thread = thread;

        LOGGER_INFO( "thread", "create thread: %s id: %llu priority: %d"
            , m_description.nameA
            , (unsigned long long)m_threadId
            , m_priority
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadProcessor::finalize()
    {
        SDL_DestroyMutex( m_taskLock );
        m_taskLock = nullptr;

        SDL_DestroyMutex( m_processLock );
        m_processLock = nullptr;

        SDL_DestroyCondition( m_conditionVariable );
        m_conditionVariable = nullptr;

        SDL_DestroyMutex( m_conditionLock );
        m_conditionLock = nullptr;

        m_thread = nullptr;
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadProcessor::main()
    {
        m_threadId = SDL_GetCurrentThreadID();

        ALLOCATOR_SYSTEM()
            ->beginThread( (ThreadId)m_threadId );

        if( PLATFORM_SYSTEM()
            ->beginThread( (ThreadId)m_threadId ) == false )
        {
            LOGGER_ERROR( "invalid begin thread '%s' id: %llu"
                , m_description.nameA
                , (unsigned long long)m_threadId
            );

            return;
        }

        LOGGER_INFO( "thread", "create thread name: %s id: %llu priority: %d"
            , m_description.nameA
            , (unsigned long long)m_threadId
            , m_priority
        );

        MENGINE_PROFILER_THREAD( m_description.nameA );

        while( m_exit == false )
        {
            SDL_LockMutex( m_conditionLock );

            bool result = SDL_WaitConditionTimeout( m_conditionVariable, m_conditionLock, 1000 );

            if( result == false )
            {
                // Timeout or error - continue waiting
            }

            SDL_UnlockMutex( m_conditionLock );

            if( m_exit == true )
            {
                break;
            }

            SDL_LockMutex( m_processLock );

            if( m_task != nullptr )
            {
                if( m_exit == false )
                {
                    m_mutex->lock();
                    m_task->process();
                    m_mutex->unlock();
                }

                SDL_LockMutex( m_taskLock );

                m_task = nullptr;

                SDL_UnlockMutex( m_taskLock );
            }

            SDL_UnlockMutex( m_processLock );
        }

        PLATFORM_SYSTEM()
            ->endThread( (ThreadId)m_threadId );

        ALLOCATOR_SYSTEM()
            ->endThread( (ThreadId)m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId SDL3ThreadProcessor::getThreadId() const
    {
        return (ThreadId)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadProcessor::processTask( const ThreadTaskInterfacePtr & _task )
    {
        if( m_exit == true )
        {
            return false;
        }

        bool processLockResult = SDL_TryLockMutex( m_processLock );

        if( processLockResult == false )
        {
            return false;
        }

        bool successful = false;

        if( m_task == nullptr )
        {
            if( _task->run( m_mutex ) == true )
            {
                SDL_LockMutex( m_taskLock );

                m_task = _task;

                SDL_UnlockMutex( m_taskLock );
            }
            else
            {
                LOGGER_ERROR( "invalid run" );
            }

            SDL_SignalCondition( m_conditionVariable );

            successful = true;
        }

        SDL_UnlockMutex( m_processLock );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadProcessor::removeTask()
    {
        if( m_exit == true )
        {
            return;
        }

        SDL_LockMutex( m_taskLock );

        if( m_task != nullptr )
        {
            m_task->cancel();
        }

        SDL_UnlockMutex( m_taskLock );

        SDL_LockMutex( m_processLock );

        m_task = nullptr;

        SDL_UnlockMutex( m_processLock );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadProcessor::join()
    {
        if( m_exit == true )
        {
            return;
        }

        m_exit = true;

        SDL_SignalCondition( m_conditionVariable );

        int status = 0;
        SDL_WaitThread( m_thread, &status );

        if( status != 0 )
        {
            LOGGER_ERROR( "join thread with error status [%d]"
                , status
            );
        }

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadProcessor::isCurrentThread() const
    {
        SDL_ThreadID threadId = SDL_GetCurrentThreadID();

        if( m_threadId != threadId )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EThreadPriority SDL3ThreadProcessor::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadDescription & SDL3ThreadProcessor::getDescription() const
    {
        return m_description;
    }
    //////////////////////////////////////////////////////////////////////////
}



