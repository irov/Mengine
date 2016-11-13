#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "SDL_Thread.h"
#	include "SDL_timer.h"

namespace Menge
{
    class SDLThreadIdentity
        : public ThreadIdentity
    {
    public:
        SDLThreadIdentity();

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, const ThreadMutexInterfacePtr & _mutex, int _priority, const char * _doc);
        
    public:
        void main();

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        bool joinTask() override;

    public:
        void join() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        
        ThreadMutexInterfacePtr m_mutex;

        const char * m_doc;

        SDL_Thread * m_thread;

        ThreadTaskInterface * m_task;
        
        volatile bool m_complete;
        volatile bool m_exit;
    };

    typedef stdex::intrusive_ptr<SDLThreadIdentity> SDLThreadIdentityPtr;
}
