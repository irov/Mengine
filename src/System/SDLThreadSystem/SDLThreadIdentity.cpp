#	include "SDLThreadIdentity.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadIdentity::SDLThreadIdentity()
        : m_serviceProvider(nullptr)
        , m_doc(nullptr)
        , m_thread(nullptr)
        , m_task(nullptr)
        , m_complete(true)
        , m_exit(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    static int s_tread_job( void * _userData )
    {
        SDLThreadIdentity * thread = reinterpret_cast<SDLThreadIdentity *>(_userData);
        
        thread->main();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentity::initialize( ServiceProviderInterface * _serviceProvider, const ThreadMutexInterfacePtr & _mutex, int _priority, const char * _doc)
    {
        (void)_priority;

        m_serviceProvider = _serviceProvider;
        m_mutex = _mutex;
        m_doc = _doc;

        SDL_Thread * thread = SDL_CreateThread( &s_tread_job, "Mengine Thread", reinterpret_cast<void *>(this) );

        if( thread == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLThreadIdentity::createThread: invalid create thread error code - %s"
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
            {
                if( m_complete == false )
                {
                    m_task->main();
                    m_complete = true;
                }

                if( m_exit == true )
                {
                    work = false;
                }
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
        }

        m_mutex->unlock();
        
        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentity::joinTask()
    {
        bool successful = false;

        m_mutex->lock();

        if( m_complete == false )
        {	
            m_task = nullptr;
            m_complete = true;

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
        {
            m_exit = true;
        }
        m_mutex->unlock();

        int status = 99;
        SDL_WaitThread( m_thread, &status );

        if( status != 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLThreadIdentity::joinThread: invalid join thread error code - %s"
                , status
                );
        }

        m_thread = nullptr;
    }
}