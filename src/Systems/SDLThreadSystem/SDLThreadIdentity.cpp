#include "SDLThreadIdentity.h"

#include "Interface/AllocatorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadEnum.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int s_treadJob( void * _userData )
        {
            SDLThreadIdentity * thread = reinterpret_cast<SDLThreadIdentity *>(_userData);

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
    SDLThreadIdentity::SDLThreadIdentity()
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
    SDLThreadIdentity::~SDLThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentity::initialize( EThreadPriority _priority, const ConstString & _name, const ThreadMutexInterfacePtr & _mutex, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        m_priority = _priority;
        m_name = _name;

        m_mutex = _mutex;

#ifdef MENGINE_DEBUG
        m_doc = _doc;
#endif

        SDL_mutex * taskLock = SDL_CreateMutex();

        if( taskLock == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error: %s"
                , SDL_GetError()
            );

            return false;
        }

        m_taskLock = taskLock;

        SDL_mutex * processLock = SDL_CreateMutex();

        if( processLock == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error: %s"
                , SDL_GetError()
            );

            return false;
        }

        m_processLock = processLock;

        SDL_mutex * conditionLock = SDL_CreateMutex();

        if( conditionLock == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error: %s"
                , SDL_GetError()
            );

            return false;
        }

        SDL_cond * conditionVariable = SDL_CreateCond();

        if( conditionVariable == nullptr )
        {
            LOGGER_ERROR( "invalid create condition error: %s"
                , SDL_GetError()
            );

            return false;
        }

        m_conditionLock = conditionLock;
        m_conditionVariable = conditionVariable;

        SDL_Thread * thread = SDL_CreateThread( &Detail::s_treadJob, m_name.c_str(), reinterpret_cast<void *>(this) );

        if( thread == nullptr )
        {
            LOGGER_ERROR( "invalid create thread error: %s"
                , SDL_GetError()
            );

            return false;
        }

        m_thread = thread;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentity::main()
    {
        m_threadId = SDL_ThreadID();

        ALLOCATOR_SERVICE()
            ->startThread();

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
                    m_task->main();
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

        ALLOCATOR_SERVICE()
            ->stopThread();
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t SDLThreadIdentity::getThreadId() const
    {
        return (uint64_t)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentity::processTask( ThreadTaskInterface * _task )
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
    void SDLThreadIdentity::removeTask()
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
    void SDLThreadIdentity::join()
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
            LOGGER_ERROR( "invalid join thread error status [%d]"
                , status
            );
        }

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
    bool SDLThreadIdentity::isCurrentThread() const
    {
        SDL_threadID threadId = SDL_ThreadID();

        if( m_threadId != threadId )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EThreadPriority SDLThreadIdentity::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
}
