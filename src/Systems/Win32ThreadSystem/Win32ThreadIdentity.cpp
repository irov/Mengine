#include "Win32ThreadIdentity.h"

#include "Interface/AllocatorSystemInterface.h"

#include "Environment/Windows/Win32Helper.h"

#include "Win32ThreadHelper.h"
#include "Win32ThreadIdentityRunner.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static DWORD WINAPI s_treadJob( LPVOID lpThreadParameter )
        {
            Win32ThreadIdentity * thread = static_cast<Win32ThreadIdentity *>(lpThreadParameter);

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
    Win32ThreadIdentity::Win32ThreadIdentity()
        : m_priority( ETP_NORMAL )
        , m_thread( INVALID_HANDLE_VALUE )
        , m_threadId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadIdentity::~Win32ThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadIdentity::initialize( const ConstString & _name, EThreadPriority _priority )
    {
        m_name = _name;
        m_priority = _priority;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::finalize()
    {
        if( m_thread != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( m_thread );
            m_thread = INVALID_HANDLE_VALUE;
        }

        m_runner = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityRunnerInterfacePtr Win32ThreadIdentity::run( const LambdaThreadRunner & _lambda )
    {
        m_runner = Helper::makeFactorableUnique<Win32ThreadIdentityRunner>( MENGINE_DOCUMENT_FACTORABLE, _lambda );

        HANDLE thread = ::CreateThread( NULL, 0, &Detail::s_treadJob, (LPVOID)this, 0, NULL );

        if( thread == NULL )
        {
            LOGGER_ERROR( "invalid CreateThread get error %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return nullptr;
        }        

#if defined(MENGINE_DEBUG) && defined(MENGINE_TOOLCHAIN_MSVC) && defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        if( ::IsDebuggerPresent() == TRUE )
        {
            DWORD threadId = ::GetThreadId( thread );
            Helper::Win32SetThreadName( threadId, m_name.c_str() );
        }
#endif

        m_thread = thread;

        switch( m_priority )
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
                LOGGER_ASSERTION( "invalid thread priority [%u]"
                    , m_priority
                );
            }break;
        }

        LOGGER_INFO( "thread", "create thread name: %s id: %ld priority: %d"
            , m_name.c_str()
            , m_threadId
            , m_priority
        );

        return m_runner;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::main()
    {        
        m_threadId = ::GetCurrentThreadId();

        ALLOCATOR_SYSTEM()
            ->beginThread( m_threadId );

        MENGINE_PROFILER_THREAD( m_name.c_str() );

        ThreadIdentityRunnerInterfacePtr runner = m_runner;
        runner->run();

        ALLOCATOR_SYSTEM()
            ->endThread( m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t Win32ThreadIdentity::getThreadId() const
    {
        return (uint64_t)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::join()
    {
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