#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "Kernel/ServantBase.h"

#	include "SDL_thread.h"
#	include "SDL_timer.h"

namespace Mengine
{
    class SDLThreadIdentity
        : public ServantBase<ThreadIdentityInterface>
    {
    public:
        SDLThreadIdentity();
        ~SDLThreadIdentity();

    public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex, int _priority, const char * _file, uint32_t _line );
        
    public:

        void main();

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        bool completeTask() override;

    public:
        void join() override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        SDL_Thread * m_thread;

        ThreadTaskInterface * m_task;

		const char * m_file;
		uint32_t m_line;
        
        bool m_complete;
		bool m_exit;
	};

    typedef stdex::intrusive_ptr<SDLThreadIdentity> SDLThreadIdentityPtr;
}
