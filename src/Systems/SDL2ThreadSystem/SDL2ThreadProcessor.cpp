#include "SDL2ThreadProcessor.h"

#include "Interface/AllocatorSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadEnum.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int treadJob( void * _userData )
        {
            SDL2ThreadProcessor * thread = reinterpret_cast<SDL2ThreadProcessor *>(_userData);

            EThreadPriority priority = thread->getPriority();

            switch( priority )
            {
            case ETP_LOWEST:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_LOW );
                }break;
            case ETP_BELOW_NORMAL:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_LOW );
                }break;
            case ETP_NORMAL:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
                }break;
            case ETP_ABOVE_NORMAL:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
                }break;
            case ETP_HIGHEST:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_HIGH );
                }break;
            case ETP_TIME_CRITICAL:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_TIME_CRITICAL );
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
    SDL2ThreadProcessor::SDL2ThreadProcessor()
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
    SDL2ThreadProcessor::~SDL2ThreadProcessor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2ThreadProcessor::initialize( const ThreadDescription & _description, EThreadPriority _priority, const ThreadMutexInterfacePtr & _mutex )
    {
        m_priority = _priority;
        m_description = _description;

        m_mutex = _mutex;

        SDL_mutex * taskLock = SDL_CreateMutex();

        if( taskLock == nullptr )
        {
            LOGGER_ERROR( "invalid create thread: %s mutex error: %s"
                , m_description.nameA
                , SDL_GetError()
            );

            return false;
        }

        m_taskLock = taskLock;

        SDL_mutex * processLock = SDL_CreateMutex();

        if( processLock == nullptr )
        {
            LOGGER_ERROR( "invalid create thread: %s mutex error: %s"
                , m_description.nameA
                , SDL_GetError()
            );

            return false;
        }

        m_processLock = processLock;

        SDL_mutex * conditionLock = SDL_CreateMutex();

        if( conditionLock == nullptr )
        {
            LOGGER_ERROR( "invalid create thread: %s mutex error: %s"
                , m_description.nameA
                , SDL_GetError()
            );

            return false;
        }

        SDL_cond * conditionVariable = SDL_CreateCond();

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

        LOGGER_INFO( "thread", "create thread: %s id: %ld priority: %d"
            , m_description.nameA
            , m_threadId
            , m_priority
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadProcessor::finalize()
    {
        SDL_DestroyMutex( m_taskLock );
        m_taskLock = nullptr;

        SDL_DestroyMutex( m_processLock );
        m_processLock = nullptr;

        SDL_DestroyCond( m_conditionVariable );
        m_conditionVariable = nullptr;

        SDL_DestroyMutex( m_conditionLock );
        m_conditionLock = nullptr;

        m_thread = nullptr;
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadProcessor::main()
    {
        m_threadId = SDL_ThreadID();

        ALLOCATOR_SYSTEM()
            ->beginThread( m_threadId );

        while( m_exit == false )
        {
            if( SDL_LockMutex( m_conditionLock ) != 0 )
            {
                LOGGER_ERROR( "invalid lock mutex error: %s"
                    , SDL_GetError()
                );
            }

            int err = SDL_CondWaitTimeout( m_conditionVariable, m_conditionLock, 1000 );

            if( err == -1 )
            {
                LOGGER_ERROR( "invalid cond wait timeout error: %s"
                    , SDL_GetError()
                );
            }

            if( SDL_UnlockMutex( m_conditionLock ) != 0 )
            {
                LOGGER_ERROR( "invalid unlock mutex error: %s"
                    , SDL_GetError()
                );
            }

            if( m_exit == true )
            {
                break;
            }

            if( SDL_LockMutex( m_processLock ) != 0 )
            {
                LOGGER_ERROR( "invalid lock mutex error: %s"
                    , SDL_GetError()
                );
            }

            if( m_task != nullptr )
            {
                if( m_exit == false )
                {
                    m_mutex->lock();
                    m_task->process();
                    m_mutex->unlock();
                }

                if( SDL_LockMutex( m_taskLock ) != 0 )
                {
                    LOGGER_ERROR( "invalid lock mutex error: %s"
                        , SDL_GetError()
                    );
                }

                m_task = nullptr;

                if( SDL_UnlockMutex( m_taskLock ) != 0 )
                {
                    LOGGER_ERROR( "invalid unlock mutex error: %s"
                        , SDL_GetError()
                    );
                }
            }

            if( SDL_UnlockMutex( m_processLock ) != 0 )
            {
                LOGGER_ERROR( "invalid unlock mutex error: %s"
                    , SDL_GetError()
                );
            }
        }

        ALLOCATOR_SYSTEM()
            ->endThread( m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId SDL2ThreadProcessor::getThreadId() const
    {
        return (ThreadId)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2ThreadProcessor::processTask( ThreadTaskInterface * _task )
    {
        if( m_exit == true )
        {
            return false;
        }

        int processLockResult = SDL_TryLockMutex( m_processLock );

        if( processLockResult != 0 )
        {
            return false;
        }

        bool successful = false;

        if( m_task == nullptr )
        {
            if( _task->run( m_mutex ) == true )
            {
                if( SDL_LockMutex( m_taskLock ) != 0 )
                {
                    LOGGER_ERROR( "invalid lock mutex error: %s"
                        , SDL_GetError()
                    );
                }

                m_task = _task;

                if( SDL_UnlockMutex( m_taskLock ) != 0 )
                {
                    LOGGER_ERROR( "invalid unlock mutex error: %s"
                        , SDL_GetError()
                    );
                }
            }
            else
            {
                LOGGER_ERROR( "invalid run" );
            }

            if( SDL_CondSignal( m_conditionVariable ) != 0 )
            {
                LOGGER_ERROR( "invalid cond signal error: %s"
                    , SDL_GetError()
                );
            }

            successful = true;
        }

        if( SDL_UnlockMutex( m_processLock ) != 0 )
        {
            LOGGER_ERROR( "invalid unlock mutex error: %s"
                , SDL_GetError()
            );
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadProcessor::removeTask()
    {
        if( m_exit == true )
        {
            return;
        }

        if( SDL_LockMutex( m_taskLock ) != 0 )
        {
            LOGGER_ERROR( "invalid lock mutex error: %s"
                , SDL_GetError()
            );
        }

        if( m_task != nullptr )
        {
            m_task->cancel();
        }

        if( SDL_UnlockMutex( m_taskLock ) != 0 )
        {
            LOGGER_ERROR( "invalid unlock mutex error: %s"
                , SDL_GetError()
            );
        }

        if( SDL_LockMutex( m_processLock ) != 0 )
        {
            LOGGER_ERROR( "invalid lock mutex error: %s"
                , SDL_GetError()
            );
        }

        m_task = nullptr;

        if( SDL_UnlockMutex( m_processLock ) != 0 )
        {
            LOGGER_ERROR( "invalid unlock mutex error: %s"
                , SDL_GetError()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadProcessor::join()
    {
        if( m_exit == true )
        {
            return;
        }

        m_exit = true;

        if( SDL_CondSignal( m_conditionVariable ) != 0 )
        {
            LOGGER_ERROR( "invalid cond signal error: %s"
                , SDL_GetError()
            );
        }

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
    bool SDL2ThreadProcessor::isCurrentThread() const
    {
        SDL_threadID threadId = SDL_ThreadID();

        if( m_threadId != threadId )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EThreadPriority SDL2ThreadProcessor::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadDescription & SDL2ThreadProcessor::getDescription() const
    {
        return m_description;
    }
    //////////////////////////////////////////////////////////////////////////
}
