#include "SDL3ThreadIdentity.h"

#include "Interface/AllocatorSystemInterface.h"
#include "Interface/PlatformSystemInterface.h"

#include "SDL3ThreadIdentityRunner.h"

#include "Kernel/Logger.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ThreadEnum.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int treadJob( void * _userData )
        {
            SDL3ThreadIdentity * thread = reinterpret_cast<SDL3ThreadIdentity *>(_userData);

            EThreadPriority priority = thread->getPriority();

            switch( priority )
            {
            case ETP_LOWEST:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_LOW );
                }break;
            case ETP_BELOW_NORMAL:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_LOW );
                }break;
            case ETP_NORMAL:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
                }break;
            case ETP_ABOVE_NORMAL:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
                }break;
            case ETP_HIGHEST:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_HIGH );
                }break;
            case ETP_TIME_CRITICAL:
                {
                    SDL_SetCurrentThreadPriority( SDL_THREAD_PRIORITY_TIME_CRITICAL );
                }break;
            default:
                break;
            }

            thread->main();

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadIdentity::SDL3ThreadIdentity()
        : m_priority( ETP_NORMAL )
        , m_threadId( 0 )
        , m_thread( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadIdentity::~SDL3ThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadIdentity::initialize( const ThreadDescription & _description, EThreadPriority _priority )
    {
        m_description = _description;
        m_priority = _priority;        

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadIdentity::finalize()
    {
        m_thread = nullptr;
        m_runner = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityRunnerInterfacePtr SDL3ThreadIdentity::run( const LambdaThreadRunner & _lambda, Timestamp _sleep, const DocumentInterfacePtr & _doc )
    {
        m_runner = Helper::makeFactorableUnique<SDL3ThreadIdentityRunner>( _doc, _lambda, _sleep );

        SDL_Thread * thread = SDL_CreateThread( &Detail::treadJob, m_description.nameA, reinterpret_cast<void *>(this) );

        if( thread == nullptr )
        {
            LOGGER_ERROR( "invalid create thread error: %s"
                , SDL_GetError()
            );

            return nullptr;
        }

        m_thread = thread;

        LOGGER_INFO( "thread", "create thread name: %s id: %llu priority: %d"
            , m_description.nameA
            , (unsigned long long)m_threadId
            , m_priority
        );

        return m_runner;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadIdentityRunnerInterfacePtr & SDL3ThreadIdentity::getRunner() const
    {
        return m_runner;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadIdentity::main()
    {
        m_threadId = SDL_GetCurrentThreadID();

        ALLOCATOR_SYSTEM()
            ->beginThread( (ThreadId)m_threadId );

        if( PLATFORM_SYSTEM()
            ->beginThread( (ThreadId)m_threadId ) == false )
        {
            LOGGER_ERROR( "invalid begin thread '%s' id: %llu"
                , m_description.nameA
                , m_threadId
            );

            return;
        }

        LOGGER_INFO( "thread", "create thread name: %s id: %llu priority: %d"
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

        PLATFORM_SYSTEM()
            ->endThread( (ThreadId)m_threadId );

        ALLOCATOR_SYSTEM()
            ->endThread( (ThreadId)m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId SDL3ThreadIdentity::getThreadId() const
    {
        return (ThreadId)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadIdentity::join( bool _cancel )
    {
        if( _cancel == true )
        {
            m_runner->cancel();
        }

        int status = 0;
        SDL_WaitThread( m_thread, &status );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid join thread '%s' error status [%d]"
                , m_description.nameA
                , status
            );
        }

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadIdentity::isCurrentThread() const
    {
        SDL_ThreadID threadId = SDL_GetCurrentThreadID();

        if( m_threadId != threadId )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EThreadPriority SDL3ThreadIdentity::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadDescription & SDL3ThreadIdentity::getDescription() const
    {
        return m_description;
    }
    //////////////////////////////////////////////////////////////////////////
}