#	include "SDLThreadIdentity.h"

#	include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadIdentity::SDLThreadIdentity()
        : m_thread( nullptr )
        , m_priority( 0 )
        , m_task( nullptr )
        , m_file( nullptr )
        , m_line( 0 )
        , m_complete( true )
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
    bool SDLThreadIdentity::initialize( const ThreadMutexInterfacePtr & _mutex, int32_t _priority, const Char * _file, uint32_t _line )
    {
        MENGINE_UNUSED( _priority );

        m_mutex = _mutex;
        m_priority = _priority;
        m_file = _file;
        m_line = _line;

        SDL_Thread * thread = SDL_CreateThread( &s_tread_job, "Mengine Thread", reinterpret_cast<void *>(this) );

        if( thread == nullptr )
        {
            LOGGER_ERROR( "SDLThreadIdentity::createThread: invalid create thread error code - %s"
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
        bool work = true;
        while( work )
        {
            m_mutex->lock();

            if( m_complete == false && m_exit == false )
            {
                m_task->main();
                m_complete = true;
            }

            if( m_exit == true )
            {
                m_complete = true;
                work = false;
            }

            m_mutex->unlock();

            SDL_Delay( 10 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentity::processTask( ThreadTaskInterface * _task )
    {
        if( m_mutex->try_lock() == false )
        {
            return false;
        }

        bool successful = false;

        if( m_complete == true && m_exit == false )
        {
            m_task = _task;
            m_complete = false;

            successful = true;

            m_task->preparation();
        }

        m_mutex->unlock();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentity::completeTask()
    {
        if( m_exit == true )
        {
            return false;
        }

        bool successful = false;

        m_mutex->lock();

        if( m_complete == false )
        {
            m_task = nullptr;

            successful = true;
        }

        m_mutex->unlock();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentity::join()
    {
        if( m_exit == true )
        {
            return;
        }

        m_mutex->lock();

        m_exit = true;

        m_mutex->unlock();

        int status;
        SDL_WaitThread( m_thread, &status );

        if( status != 0 )
        {
            LOGGER_ERROR( "SDLThreadIdentity::joinThread: invalid join thread error code - %s"
                , status
            );
        }

        m_thread = nullptr;
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLThreadIdentity::getPriority() const
    {
        return m_priority;
    }
}
