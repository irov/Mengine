#	include "SDLThreadIdentity.h"

#	include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadIdentity::SDLThreadIdentity()
        : m_priority( 0 )
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
    static int s_tread_job( void * _userData )
    {
        SDLThreadIdentity * thread = reinterpret_cast<SDLThreadIdentity *>(_userData);

        int32_t priority = thread->getPriority();

        switch( priority )
        {
        case MENGINE_THREAD_PRIORITY_LOWEST:
            {
                SDL_SetThreadPriority( SDL_THREAD_PRIORITY_LOW );
            }break;
        case MENGINE_THREAD_PRIORITY_BELOW_NORMAL:
            {
                SDL_SetThreadPriority( SDL_THREAD_PRIORITY_LOW );
            }break;
        case MENGINE_THREAD_PRIORITY_NORMAL:
            {
                SDL_SetThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
            }break;
        case MENGINE_THREAD_PRIORITY_ABOVE_NORMAL:
            {
                SDL_SetThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
            }break;
        case MENGINE_THREAD_PRIORITY_HIGHEST:
            {
                SDL_SetThreadPriority( SDL_THREAD_PRIORITY_HIGH );
            }break;
        case MENGINE_THREAD_PRIORITY_TIME_CRITICAL:
            {
                SDL_SetThreadPriority( SDL_THREAD_PRIORITY_TIME_CRITICAL );
            }break;
        }

        thread->main();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentity::initialize( int32_t _priority, const ConstString & _name, const ThreadMutexInterfacePtr & _mutex, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        m_priority = _priority;
        m_name = _name;

        m_mutex = _mutex;

#ifdef MENGINE_DEBUG
        m_doc = _doc;
#endif

        SDL_mutex * processLock = SDL_CreateMutex();

        if( processLock == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error code - '%s'"
                , SDL_GetError()
            );

            return false;
        }

        m_processLock = processLock;

        SDL_mutex * conditionLock = SDL_CreateMutex();

        if( conditionLock == nullptr )
        {
            LOGGER_ERROR( "invalid create mutex error code - '%s'"
                , SDL_GetError()
            );

            return false;
        }

        SDL_cond * conditionVariable = SDL_CreateCond();

        if( conditionVariable == nullptr )
        {
            LOGGER_ERROR( "invalid create condition error code - %s"
                , SDL_GetError()
            );

            return false;
        }

        m_conditionLock = conditionLock;
        m_conditionVariable = conditionVariable;

        SDL_Thread * thread = SDL_CreateThread( &s_tread_job, m_name.c_str(), reinterpret_cast<void *>(this) );

        if( thread == nullptr )
        {
            LOGGER_ERROR( "invalid create thread error code - '%s'"
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
        for( ; m_exit == false; )
        {
            SDL_LockMutex( m_conditionLock );
            SDL_CondWaitTimeout( m_conditionVariable, m_conditionLock, 1000 );
            SDL_UnlockMutex( m_conditionLock );

            SDL_LockMutex( m_processLock );

            if( m_task != nullptr )
            {
                if( m_exit == false )
                {
                    m_mutex->lock();
                    m_task->main();
                    m_mutex->unlock();
                }

                SDL_LockMutex( m_taskLock );
                m_task = nullptr;
                SDL_UnlockMutex( m_taskLock );
            }

            SDL_UnlockMutex( m_processLock );
        }
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
                SDL_LockMutex( m_taskLock );
                m_task = _task;
                SDL_UnlockMutex( m_taskLock );
            }
            else
            {
                LOGGER_ERROR( "invalid run"
                );
            }

            SDL_CondSignal( m_conditionVariable );

            successful = true;
        }

        SDL_UnlockMutex( m_processLock );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentity::removeTask()
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
    void SDLThreadIdentity::join()
    {
        if( m_exit == true )
        {
            return;
        }

        m_exit = true;

        SDL_CondSignal( m_conditionVariable );

        int status;
        SDL_WaitThread( m_thread, &status );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid join thread error code - '%d'"
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
    int32_t SDLThreadIdentity::getPriority() const
    {
        return m_priority;
    }
}
