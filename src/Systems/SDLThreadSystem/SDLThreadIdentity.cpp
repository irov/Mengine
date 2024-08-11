#include "SDLThreadIdentity.h"

#include "Interface/AllocatorSystemInterface.h"

#include "SDLThreadIdentityRunner.h"

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
        static int s_treadJob( void * _userData )
        {
            SDLThreadIdentity * thread = reinterpret_cast<SDLThreadIdentity *>(_userData);

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
    SDLThreadIdentity::SDLThreadIdentity()
        : m_priority( ETP_NORMAL )
        , m_threadId( 0 )
        , m_thread( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadIdentity::~SDLThreadIdentity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentity::initialize( EThreadPriority _priority, const ConstString & _name )
    {
        m_priority = _priority;
        m_name = _name;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentity::finalize()
    {
        m_thread = nullptr;
        m_runner = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityRunnerInterfacePtr SDLThreadIdentity::run( const LambdaThreadRunner & _lambda )
    {
        m_runner = Helper::makeFactorableUnique<SDLThreadIdentityRunner>( MENGINE_DOCUMENT_FACTORABLE, _lambda );

        SDL_Thread * thread = SDL_CreateThread( &Detail::s_treadJob, m_name.c_str(), reinterpret_cast<void *>(this) );

        if( thread == nullptr )
        {
            LOGGER_ERROR( "invalid create thread error: %s"
                , SDL_GetError()
            );

            return nullptr;
        }

        m_thread = thread;

        LOGGER_INFO( "thread", "create thread name: %s id: %ld priority: %d"
            , m_name.c_str()
            , m_threadId
            , m_priority
        );

        return m_runner;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentity::main()
    {
        m_threadId = SDL_ThreadID();

        ALLOCATOR_SYSTEM()
            ->beginThread( m_threadId );

        MENGINE_PROFILER_THREAD( m_name.c_str() );
        
        ThreadIdentityRunnerInterfacePtr runner = m_runner;
        runner->run();

        ALLOCATOR_SYSTEM()
            ->endThread( m_threadId );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId SDLThreadIdentity::getThreadId() const
    {
        return (ThreadId)m_threadId;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadIdentity::join()
    {
        if( m_runner->isCancel() == true )
        {
            return;
        }

        int status = 0;
        SDL_WaitThread( m_thread, &status );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid join thread error status [%d]"
                , status
            );
        }

        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadIdentity::isCurrentThread() const
    {
        SDL_threadID threadId = SDL_ThreadID();

        if( m_threadId != threadId )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EThreadPriority SDLThreadIdentity::getPriority() const
    {
        return m_priority;
    }
    //////////////////////////////////////////////////////////////////////////
}
