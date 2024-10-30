#include "Win32ThreadIdentity.h"

#include "Interface/AllocatorSystemInterface.h"
#include "Interface/Win32KernelServiceInterface.h"

#include "Environment/Windows/Win32Helper.h"

#include "Win32ThreadIdentityRunner.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/FactorableUnique.h"

#include "Config/StdString.h"

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
    bool Win32ThreadIdentity::initialize( const ThreadDescription & _description, EThreadPriority _priority )
    {           
        m_description = _description;
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
    ThreadIdentityRunnerInterfacePtr Win32ThreadIdentity::run( const LambdaThreadRunner & _lambda, Timestamp _sleep, const DocumentInterfacePtr & _doc )
    {
        m_runner = Helper::makeFactorableUnique<Win32ThreadIdentityRunner>( _doc, _lambda, _sleep );

        HANDLE thread = ::CreateThread( NULL, 0, &Detail::s_treadJob, (LPVOID)this, 0, NULL );

        if( thread == NULL )
        {
            LOGGER_ERROR( "invalid CreateThread get error %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return nullptr;
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

        return m_runner;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::main()
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

        ThreadIdentityRunnerInterfacePtr runner = m_runner;

        while( runner->run() == true && runner->isCancel() == false )
        {
            runner->sleep();
        }

        ALLOCATOR_SYSTEM()
            ->endThread( m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t Win32ThreadIdentity::getThreadId() const
    {
        return (uint64_t)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::join( bool _cancel )
    {
        if( _cancel == true )
        {
            m_runner->cancel();
        }

        ::WaitForSingleObject( m_thread, INFINITE );

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadIdentityRunnerInterfacePtr & Win32ThreadIdentity::getRunner() const
    {
        return m_runner;
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
    EThreadPriority Win32ThreadIdentity::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadDescription & Win32ThreadIdentity::getDescription() const
    {
        return m_description;
    }
    //////////////////////////////////////////////////////////////////////////
}