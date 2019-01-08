#include "Win32ThreadIdentity.h"

#include "Kernel/Logger.h"

#include <process.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)  
        typedef struct tagTHREADNAME_INFO
        {
            DWORD dwType; // Must be 0x1000.  
            LPCSTR szName; // Pointer to name (in user addr space).  
            DWORD dwThreadID; // Thread ID (-1=caller thread).  
            DWORD dwFlags; // Reserved for future use, must be zero.  
        } THREADNAME_INFO;
#pragma pack(pop)  
        void SetThreadName( DWORD dwThreadID, const char* threadName ) {
            THREADNAME_INFO info;
            info.dwType = 0x1000;
            info.szName = threadName;
            info.dwThreadID = dwThreadID;
            info.dwFlags = 0;
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
            __try {
                RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), (ULONG_PTR*)&info );
            }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
            }
#pragma warning(pop)  
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentity::Win32ThreadIdentity()
        : m_thread( INVALID_HANDLE_VALUE )
        , m_task( nullptr )
        , m_complete( true )
        , m_exit( false )
#ifndef NDEBUG
        , m_file( nullptr )
        , m_line( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentity::~Win32ThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    static uint32_t __stdcall s_tread_job( void * _userData )
    {
        Win32ThreadIdentity * thread = static_cast<Win32ThreadIdentity*>(_userData);

#ifdef NDEBUG
        try
#endif
        {
            thread->main();
        }
#ifdef NDEBUG
        catch( const std::exception & ex )
        {
            LOGGER_CRITICAL( "Win32ThreadIdentity std::exception exception '%s'"
                , ex.what()
            );
        }
        catch( ... )
        {
            LOGGER_CRITICAL( "Win32ThreadIdentity unknown exception"
            );
        }
#endif

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::initialize( const ThreadMutexInterfacePtr & _mutex, int32_t _priority, const Char * _doc, const Char * _file, uint32_t _line )
    {
        MENGINE_UNUSED( _file );
        MENGINE_UNUSED( _line );

        m_mutex = _mutex;

#ifndef NDEBUG
        m_file = _file;
        m_line = _line;
#endif

        m_thread = (HANDLE)::_beginthreadex( NULL, 0, &s_tread_job, (LPVOID)this, 0, NULL );

        if( m_thread == NULL )
        {
            DWORD error_code = GetLastError();

            LOGGER_ERROR( "Win32ThreadIdentity::initialize: invalid create thread error code - %d"
                , error_code
            );

            return false;
        }

        DWORD threadId = ::GetThreadId( m_thread );

#if defined(_MSC_VER)
        Detail::SetThreadName( threadId, _doc );
#endif

        switch( _priority )
        {
        case MENGINE_THREAD_PRIORITY_LOWEST:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_LOWEST );
            }break;
        case MENGINE_THREAD_PRIORITY_BELOW_NORMAL:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_BELOW_NORMAL );
            }break;
        case MENGINE_THREAD_PRIORITY_NORMAL:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_NORMAL );
            }break;
        case MENGINE_THREAD_PRIORITY_ABOVE_NORMAL:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_ABOVE_NORMAL );
            }break;
        case MENGINE_THREAD_PRIORITY_HIGHEST:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_HIGHEST );
            }break;
        case MENGINE_THREAD_PRIORITY_TIME_CRITICAL:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_TIME_CRITICAL );
            }break;
        }

        InitializeCriticalSection( &m_conditionLock );
        InitializeConditionVariable( &m_conditionVariable );        

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::main()
    {
        bool work = true;

        while( work == true )
        {
            m_mutex->lock();

            if( m_complete == false )
            {
                m_task->main();
                m_task = nullptr;

                m_complete = true;
            }

            m_mutex->unlock();

            if( m_exit == false )
            {
                SleepConditionVariableCS( &m_conditionVariable, &m_conditionLock, INFINITE );
            }
            
            if( m_exit == true )
            {
                work = false;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::processTask( ThreadTaskInterface * _task )
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

        WakeConditionVariable( &m_conditionVariable );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::completeTask()
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
            m_complete = true;

            successful = true;
        }

        m_mutex->unlock();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::join()
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
        
        WakeAllConditionVariable( &m_conditionVariable );
        
        WaitForSingleObject( m_thread, INFINITE );
        CloseHandle( m_thread );
        m_thread = INVALID_HANDLE_VALUE;

        DeleteCriticalSection( &m_conditionLock );
    }
}