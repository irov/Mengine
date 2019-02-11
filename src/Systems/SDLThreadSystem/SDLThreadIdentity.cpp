#	include "SDLThreadIdentity.h"

#	include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadIdentity::SDLThreadIdentity()
        : m_thread( nullptr )
		, m_processLock( nullptr )
        , m_conditionVariable( nullptr )
        , m_conditionLock( nullptr )
        , m_priority( 0 )
        , m_task( nullptr )		
        , m_exit( false )
#ifndef NDEBUG
        , m_file( nullptr )
        , m_line( 0 )
#endif
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
    bool SDLThreadIdentity::initialize( int32_t _priority, const Char * _doc, const Char * _file, uint32_t _line )
    {
        MENGINE_UNUSED( _file );
        MENGINE_UNUSED( _line );

        m_priority = _priority;

#ifndef NDEBUG
        m_file = _file;
        m_line = _line;
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
        
        SDL_Thread * thread = SDL_CreateThread( &s_tread_job, _doc, reinterpret_cast<void *>(this) );
        
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

			if( m_task != nullptr && m_exit == false )
            {
                m_task->main();
                m_task = nullptr;
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

        m_task = _task;

		SDL_UnlockMutex( m_processLock );

        SDL_CondSignal( m_conditionVariable );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentity::removeTask()
    {
        if( m_exit == true )
        {
            return;
        }

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
            LOGGER_ERROR( "invalid join thread error code - '%s'"
                , status
            );
        }

		SDL_DestroyMutex( m_processLock );
		m_processLock = nullptr;

        SDL_DestroyCond( m_conditionVariable );
        m_conditionVariable = nullptr;

        SDL_DestroyMutex( m_conditionLock );
        m_conditionLock = nullptr;

        m_thread = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLThreadIdentity::getPriority() const
    {
        return m_priority;
    }
}
