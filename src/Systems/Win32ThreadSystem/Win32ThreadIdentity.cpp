#include "Win32ThreadIdentity.h"

#include "Interface/AllocatorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/Win32Helper.h"

#include <process.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG) && defined(MENGINE_TOOLCHAIN_MSVC) && (MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_VISTA)
        //////////////////////////////////////////////////////////////////////////
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
        void SetThreadName( DWORD dwThreadID, const char * threadName )
        {
            THREADNAME_INFO info;
            info.dwType = 0x1000;
            info.szName = threadName;
            info.dwThreadID = dwThreadID;
            info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)  
            __try
            {
                ::RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), (ULONG_PTR *)&info );
            }
            __except( EXCEPTION_EXECUTE_HANDLER )
            {
            }
#pragma warning(pop)  
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        static DWORD WINAPI s_treadJob( LPVOID lpThreadParameter )
        {
            Win32ThreadIdentity * thread = static_cast<Win32ThreadIdentity *>(lpThreadParameter);

#ifdef MENGINE_DEBUG
            try
#endif
            {
                thread->main();
            }
#ifdef MENGINE_DEBUG
            catch( const std::exception & ex )
            {
                LOGGER_ERROR( "std::exception exception '%s'"
                    , ex.what()
                );
            }
            catch( ... )
            {
                LOGGER_ERROR( "unknown exception" );
            }
#endif

            ::ExitThread( 0 );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentity::Win32ThreadIdentity()
        : m_thread( INVALID_HANDLE_VALUE )
        , m_threadId( 0 )
        , m_task( nullptr )
        , m_exit( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentity::~Win32ThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::initialize( const ConstString & _name, EThreadPriority _priority, const ThreadMutexInterfacePtr & _mutex, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        m_name = _name;

#if MENGINE_DOCUMENT_ENABLE
        m_doc = _doc;
#endif

        m_mutex = _mutex;

        ::InitializeCriticalSection( &m_taskLock );
        ::InitializeCriticalSection( &m_processLock );

#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_LONGHORN
        ::InitializeCriticalSection( &m_conditionLock );
        ::InitializeConditionVariable( &m_conditionVariable );
#endif

        HANDLE thread = ::CreateThread( NULL, 0, &Detail::s_treadJob, (LPVOID)this, 0, NULL );

        if( thread == NULL )
        {
            LOGGER_ERROR( "invalid create thread %s"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        m_thread = thread;

#if defined(MENGINE_DEBUG) && defined(MENGINE_TOOLCHAIN_MSVC) && (MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_VISTA)
        if( ::IsDebuggerPresent() == TRUE )
        {
            DWORD threadId = ::GetThreadId( m_thread );
            Detail::SetThreadName( threadId, _name.c_str() );
        }
#endif

        switch( _priority )
        {
        case ETP_LOWEST:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_LOWEST );
            }break;
        case ETP_BELOW_NORMAL:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_BELOW_NORMAL );
            }break;
        case ETP_NORMAL:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_NORMAL );
            }break;
        case ETP_ABOVE_NORMAL:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_ABOVE_NORMAL );
            }break;
        case ETP_HIGHEST:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_HIGHEST );
            }break;
        case ETP_TIME_CRITICAL:
            {
                ::SetThreadPriority( m_thread, THREAD_PRIORITY_TIME_CRITICAL );
            }break;
        default:
            {
                LOGGER_ERROR( "invalid thread priority [%u]"
                    , _priority
                );
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::finalize()
    {
        ::CloseHandle( m_thread );
        m_thread = INVALID_HANDLE_VALUE;

        ::DeleteCriticalSection( &m_taskLock );
        ::DeleteCriticalSection( &m_processLock );

#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_LONGHORN
        ::DeleteCriticalSection( &m_conditionLock );
#endif

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::main()
    {
        DWORD threadId = ::GetCurrentThreadId();

        m_threadId = (uint64_t)threadId;

        ALLOCATOR_SERVICE()
            ->startThread();

        MENGINE_PROFILER_THREAD( m_name.c_str() );

        while( m_exit == false )
        {
#if MENGINE_WINDOWS_VERSION >= _WIN32_WINNT_LONGHORN
            ::EnterCriticalSection( &m_conditionLock );
            ::SleepConditionVariableCS( &m_conditionVariable, &m_conditionLock, 1000 );
            ::LeaveCriticalSection( &m_conditionLock );
#endif

            if( m_exit == true )
            {
                break;
            }

            ::EnterCriticalSection( &m_processLock );

            if( m_task != nullptr )
            {
                if( m_exit == false )
                {
                    m_mutex->lock();

                    MENGINE_PROFILER_FRAME( "thread" );

                    m_task->main();
                    m_mutex->unlock();
                }

                ::EnterCriticalSection( &m_taskLock );
                m_task = nullptr;
                ::LeaveCriticalSection( &m_taskLock );
            }

            ::LeaveCriticalSection( &m_processLock );

            if( m_exit == true )
            {
                break;
            }

#if MENGINE_WINDOWS_VERSION < _WIN32_WINNT_LONGHORN
            ::Sleep( 10 );
#endif
        }

        ALLOCATOR_SERVICE()
            ->stopThread();
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t Win32ThreadIdentity::getThreadId() const
    {
        return m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::processTask( ThreadTaskInterface * _task )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _task );

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
                ::EnterCriticalSection( &m_taskLock );
                m_task = _task;
                ::LeaveCriticalSection( &m_taskLock );
            }
            else
            {
                LOGGER_ERROR( "invalid run" );
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

        ::EnterCriticalSection( &m_taskLock );
        if( m_task != nullptr )
        {
            m_task->cancel();
        }
        ::LeaveCriticalSection( &m_taskLock );

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

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::isCurrentThread() const
    {
        HANDLE handle = ::GetCurrentThread();

        if( m_thread != handle )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}