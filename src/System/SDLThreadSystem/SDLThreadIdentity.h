#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "Core/ServantBase.h"

#	include "SDL_Thread.h"
#	include "SDL_timer.h"

namespace Menge
{
    class SDLThreadIdentity
        : public ServantBase<ThreadIdentityInterface>
    {
    public:
        SDLThreadIdentity();

    public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex, int _priority, const char * _file, uint32_t _line );
        
    public:
        void main();

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        bool joinTask() override;

    public:
        void join() override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        SDL_Thread * m_thread;

        ThreadTaskInterface * m_task;

		const char * m_file;
		uint32_t m_line;
        
        volatile bool m_complete;
		volatile bool m_exit;
	};

    typedef stdex::intrusive_ptr<SDLThreadIdentity> SDLThreadIdentityPtr;
}
