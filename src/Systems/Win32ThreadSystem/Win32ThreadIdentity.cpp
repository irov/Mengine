#include "Win32ThreadIdentity.h"

#include "Kernel/Logger.h"

#include <process.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_TOOLCHAIN_MSVC)
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
        , m_exit( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentity::~Win32ThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    static DWORD WINAPI s_tread_job( LPVOID lpThreadParameter )
    {
        Win32ThreadIdentity * thread = static_cast<Win32ThreadIdentity*>(lpThreadParameter);

#ifdef MENGINE_DEBUG
        try
#endif
        {
            thread->main();
        }
#ifdef MENGINE_DEBUG
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

        ExitThread( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::initialize( const ConstString & _name, int32_t _priority, const ThreadMutexInterfacePtr & _mutex, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        m_name = _name;

#ifdef MENGINE_DEBUG
        m_doc = _doc;
#endif

        m_mutex = _mutex;

        ::InitializeCriticalSection( &m_processLock );

#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_LONGHORN
        ::InitializeCriticalSection( &m_conditionLock );
        ::InitializeConditionVariable( &m_conditionVariable );
#endif

        m_thread = ::CreateThread( NULL, 0, &s_tread_job, (LPVOID)this, 0, NULL );

        if( m_thread == NULL )
        {
            DWORD error_code = GetLastError();

            LOGGER_ERROR( "invalid create thread error code - %d"
                , error_code
            );

            return false;
        }

#if defined(MENGINE_TOOLCHAIN_MSVC)
        DWORD threadId = ::GetThreadId( m_thread );
        Detail::SetThreadName( threadId, _name.c_str() );
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::main()
    {
        while( m_exit == false )
        {
#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_LONGHORN
            ::EnterCriticalSection( &m_conditionLock );
            ::SleepConditionVariableCS( &m_conditionVariable, &m_conditionLock, 1000 );
            ::LeaveCriticalSection( &m_conditionLock );
#else
            ::Sleep( 10 );
#endif

            ::EnterCriticalSection( &m_processLock );
            if( m_task != nullptr && m_exit == false )
            {
                m_mutex->lock();
                m_task->main();
                m_mutex->unlock();
                m_task = nullptr;
            }
            ::LeaveCriticalSection( &m_processLock );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::processTask( ThreadTaskInterface * _task )
    {
        if( m_exit == true )
        {
            return false;
        }

        if( ::TryEnterCriticalSection( &m_processLock ) == FALSE )
        {
            return false;
        }

        bool successful = false;

        if( m_task == nullptr )
        {
            if( _task->run( m_mutex ) == true )
            {
                m_task = _task;
            }
            else
            {
                LOGGER_ERROR( "invalid run"
                );
            }

#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_LONGHORN
            ::WakeConditionVariable( &m_conditionVariable );
#endif

            successful = true;
        }

        ::LeaveCriticalSection( &m_processLock );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::removeTask()
    {
        if( m_exit == true )
        {
            return;
        }

        ::EnterCriticalSection( &m_processLock );
        m_task = nullptr;
        ::LeaveCriticalSection( &m_processLock );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::join()
    {
        if( m_exit == true )
        {
            return;
        }

        m_exit = true;

#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_LONGHORN
        ::WakeConditionVariable( &m_conditionVariable );
#endif

        ::WaitForSingleObject( m_thread, INFINITE );
        ::CloseHandle( m_thread );
        m_thread = INVALID_HANDLE_VALUE;

        ::DeleteCriticalSection( &m_processLock );

#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_LONGHORN
        ::DeleteCriticalSection( &m_conditionLock );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::detach()
    {
        if( m_exit == true )
        {
            return;
        }

        m_exit = true;

        ::CloseHandle( m_thread );
        m_thread = INVALID_HANDLE_VALUE;

        ::DeleteCriticalSection( &m_processLock );

#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_LONGHORN
        ::DeleteCriticalSection( &m_conditionLock );
#endif

        m_task = nullptr;
    }

}