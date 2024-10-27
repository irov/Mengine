#include "Win32ThreadProcessor.h"

#include "Interface/AllocatorSystemInterface.h"
#include "Interface/Win32KernelServiceInterface.h"

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ProfilerHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static DWORD WINAPI s_treadJob( LPVOID lpThreadParameter )
        {
            Win32ThreadProcessor * thread = static_cast<Win32ThreadProcessor *>(lpThreadParameter);

#if defined(MENGINE_DEBUG)
            try
#endif
            {
                thread->main();
            }
#if defined(MENGINE_DEBUG)
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
    Win32ThreadProcessor::Win32ThreadProcessor()
        : m_priority( ETP_NORMAL )
        , m_thread( INVALID_HANDLE_VALUE )
        , m_threadId( 0 )
        , m_task( nullptr )
        , m_exit( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadProcessor::~Win32ThreadProcessor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadProcessor::initialize( const ThreadDescription & _description, EThreadPriority _priority, const ThreadMutexInterfacePtr & _mutex )
    {
        m_description = _description;
        m_priority = _priority;

        m_mutex = _mutex;

        ::InitializeCriticalSection( &m_taskLock );
        ::InitializeCriticalSection( &m_processLock );

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        ::InitializeCriticalSection( &m_conditionLock );
        ::InitializeConditionVariable( &m_conditionVariable );
#endif

        HANDLE thread = ::CreateThread( NULL, 0, &Detail::s_treadJob, (LPVOID)this, 0, NULL );

        if( thread == NULL )
        {
            LOGGER_ERROR( "invalid CreateThread get error %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        WIN32_KERNEL_SERVICE()
            ->setThreadDescription( thread, m_description );

        switch( m_priority )
        {
        case ETP_LOWEST:
            {
                ::SetThreadPriority( thread, THREAD_PRIORITY_LOWEST );
            }break;
        case ETP_BELOW_NORMAL:
            {
                ::SetThreadPriority( thread, THREAD_PRIORITY_BELOW_NORMAL );
            }break;
        case ETP_NORMAL:
            {
                ::SetThreadPriority( thread, THREAD_PRIORITY_NORMAL );
            }break;
        case ETP_ABOVE_NORMAL:
            {
                ::SetThreadPriority( thread, THREAD_PRIORITY_ABOVE_NORMAL );
            }break;
        case ETP_HIGHEST:
            {
                ::SetThreadPriority( thread, THREAD_PRIORITY_HIGHEST );
            }break;
        case ETP_TIME_CRITICAL:
            {
                ::SetThreadPriority( thread, THREAD_PRIORITY_TIME_CRITICAL );
            }break;
        default:
            {
                LOGGER_ASSERTION( "invalid thread priority [%u]"
                    , m_priority
                );
            }break;
        }

        m_thread = thread;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadProcessor::finalize()
    {
        if( m_thread != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( m_thread );
            m_thread = INVALID_HANDLE_VALUE;
        }

        ::DeleteCriticalSection( &m_taskLock );
        ::DeleteCriticalSection( &m_processLock );

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        ::DeleteCriticalSection( &m_conditionLock );
#endif

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadProcessor::main()
    {
        m_threadId = ::GetCurrentThreadId();

        ALLOCATOR_SYSTEM()
            ->beginThread( m_threadId );

        LOGGER_INFO( "thread", "create thread name: %s id: %ld priority: %d"
            , m_description.nameA
            , m_threadId
            , m_priority
        );

        MENGINE_PROFILER_THREAD( m_description.nameA );

        while( m_exit == false )
        {
#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
            ::EnterCriticalSection( &m_conditionLock );
            ::SleepConditionVariableCS( &m_conditionVariable, &m_conditionLock, 1000 );
            ::LeaveCriticalSection( &m_conditionLock );
#endif
            //cppcheck-suppress oppositeInnerCondition
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

            //cppcheck-suppress oppositeInnerCondition
            if( m_exit == true )
            {
                break;
            }

#if !defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
            ::Sleep( 10 );
#endif
        }

        ALLOCATOR_SYSTEM()
            ->endThread( m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId Win32ThreadProcessor::getThreadId() const
    {
        return (ThreadId)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadProcessor::processTask( ThreadTaskInterface * _task )
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
                LOGGER_ASSERTION( "invalid run" );
            }

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
            ::WakeConditionVariable( &m_conditionVariable );
#endif

            successful = true;
        }

        ::LeaveCriticalSection( &m_processLock );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadProcessor::removeTask()
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
    void Win32ThreadProcessor::join()
    {
        if( m_exit == true )
        {
            return;
        }

        m_exit = true;

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        ::WakeConditionVariable( &m_conditionVariable );
#endif

        ::WaitForSingleObject( m_thread, INFINITE );

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadProcessor::isCurrentThread() const
    {
        DWORD threadId = ::GetCurrentThreadId();

        if( m_threadId != threadId )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EThreadPriority Win32ThreadProcessor::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadDescription & Win32ThreadProcessor::getDescription() const
    {
        return m_description;
    }
    //////////////////////////////////////////////////////////////////////////
}