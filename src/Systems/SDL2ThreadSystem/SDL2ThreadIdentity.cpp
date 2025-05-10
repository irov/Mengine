#include "SDL2ThreadIdentity.h"

#include "Interface/AllocatorSystemInterface.h"

#include "SDL2ThreadIdentityRunner.h"

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
            SDL2ThreadIdentity * thread = reinterpret_cast<SDL2ThreadIdentity *>(_userData);

            EThreadPriority priority = thread->getPriority();

            switch( priority )
            {
            case ETP_LOWEST:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_LOW );
                }break;
            case ETP_BELOW_NORMAL:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_LOW );
                }break;
            case ETP_NORMAL:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
                }break;
            case ETP_ABOVE_NORMAL:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_NORMAL );
                }break;
            case ETP_HIGHEST:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_HIGH );
                }break;
            case ETP_TIME_CRITICAL:
                {
                    SDL_SetThreadPriority( SDL_THREAD_PRIORITY_TIME_CRITICAL );
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
    SDL2ThreadIdentity::SDL2ThreadIdentity()
        : m_priority( ETP_NORMAL )
        , m_threadId( 0 )
        , m_thread( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2ThreadIdentity::~SDL2ThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2ThreadIdentity::initialize( const ThreadDescription & _description, EThreadPriority _priority )
    {
        m_description = _description;
        m_priority = _priority;        

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadIdentity::finalize()
    {
        m_thread = nullptr;
        m_runner = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityRunnerInterfacePtr SDL2ThreadIdentity::run( const LambdaThreadRunner & _lambda, Timestamp _sleep, const DocumentInterfacePtr & _doc )
    {
        m_runner = Helper::makeFactorableUnique<SDL2ThreadIdentityRunner>( _doc, _lambda, _sleep );

        SDL_Thread * thread = SDL_CreateThread( &Detail::treadJob, m_description.nameA, reinterpret_cast<void *>(this) );

        if( thread == nullptr )
        {
            LOGGER_ERROR( "invalid create thread error: %s"
                , SDL_GetError()
            );

            return nullptr;
        }

        m_thread = thread;

        LOGGER_INFO( "thread", "create thread name: %s id: %ld priority: %d"
            , m_description.nameA
            , m_threadId
            , m_priority
        );

        return m_runner;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadIdentityRunnerInterfacePtr & SDL2ThreadIdentity::getRunner() const
    {
        return m_runner;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadIdentity::main()
    {
        m_threadId = SDL_ThreadID();

        ALLOCATOR_SYSTEM()
            ->beginThread( m_threadId );

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
    ThreadId SDL2ThreadIdentity::getThreadId() const
    {
        return (ThreadId)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadIdentity::join( bool _cancel )
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
    bool SDL2ThreadIdentity::isCurrentThread() const
    {
        SDL_threadID threadId = SDL_ThreadID();

        if( m_threadId != threadId )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EThreadPriority SDL2ThreadIdentity::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadDescription & SDL2ThreadIdentity::getDescription() const
    {
        return m_description;
    }
    //////////////////////////////////////////////////////////////////////////
}
