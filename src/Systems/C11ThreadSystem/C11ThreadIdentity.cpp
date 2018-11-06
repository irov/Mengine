#include "C11ThreadIdentity.h"

#include "Logger/Logger.h"

#ifdef _WIN32
#include "WIN32/WindowsIncluder.h"

#include <process.h>
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    C11ThreadIdentity::C11ThreadIdentity()
        : m_task( nullptr )
        , m_complete( true )
        , m_exit( false )
#ifndef NDEBUG
        , m_file( nullptr )
        , m_line( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    C11ThreadIdentity::~C11ThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_tread_job( C11ThreadIdentity * thread )
    {
#ifdef NDEBUG
        try
#endif
        {
            thread->main();
        }
#ifdef NDEBUG
        catch( const std::exception & ex )
        {
            LOGGER_CRITICAL( "C11ThreadIdentity std::exception exception '%s'"
                , ex.what()
            );
        }
        catch( ... )
        {
            LOGGER_CRITICAL( "C11ThreadIdentity unknown exception"
            );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool C11ThreadIdentity::initialize( int _priority, const Char * _file, uint32_t _line )
    {
        (void)_file;
        (void)_line;
        
#ifndef NDEBUG
        m_file = _file;
        m_line = _line;
#endif

        m_thread = std::thread( &s_tread_job, this );

#if WIN32
        std::thread::native_handle_type handle = m_thread.native_handle();

        if( (HANDLE)handle == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "C11ThreadIdentity::initialize: invalid create thread"
            );

            return false;
        }

        switch( _priority )
        {
        case -2:
            {
                SetThreadPriority( (HANDLE)handle, THREAD_PRIORITY_LOWEST );
            }break;
        case -1:
            {
                SetThreadPriority( (HANDLE)handle, THREAD_PRIORITY_BELOW_NORMAL );
            }break;
        case 0:
            {
                SetThreadPriority( (HANDLE)handle, THREAD_PRIORITY_NORMAL );
            }break;
        case 1:
            {
                SetThreadPriority( (HANDLE)handle, THREAD_PRIORITY_ABOVE_NORMAL );
            }break;
        case 2:
            {
                SetThreadPriority( (HANDLE)handle, THREAD_PRIORITY_HIGHEST );
            }break;
        case 3:
            {
                SetThreadPriority( (HANDLE)handle, THREAD_PRIORITY_TIME_CRITICAL );
            }break;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void C11ThreadIdentity::main()
    {
        bool work = true;

        while( work == true )
        {
            m_mutex.lock();

            if( m_complete == false )
            {
                m_task->main();

                m_task = nullptr;

                m_complete = true;
            }

            if( m_exit == true )
            {
                work = false;
            }

            m_mutex.unlock();

            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
            //Sleep( 10 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool C11ThreadIdentity::processTask( ThreadTaskInterface * _task )
    {
        if( m_mutex.try_lock() == false )
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

        m_mutex.unlock();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool C11ThreadIdentity::completeTask()
    {
        if( m_exit == true )
        {
            return false;
        }

        bool successful = false;

        if( m_complete == false )
        {
            m_task = nullptr;
            m_complete = true;

            successful = true;
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void C11ThreadIdentity::join()
    {
        if( m_exit == true )
        {
            return;
        }

        m_exit = true;

        m_thread.join();
    }
}