#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Factorable.h"
#include "Config/Atomic.h"

#include "SDL_thread.h"
#include "SDL_timer.h"

namespace Mengine
{
    class SDLThreadIdentity
        : public ThreadIdentityInterface
		, public Factorable
    {
    public:
        SDLThreadIdentity();
        ~SDLThreadIdentity() override;

    public:
        bool initialize( int32_t _priority, const Char * _doc, const Char * _file, uint32_t _line );
        
    public:
        void main();

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        void removeTask() override;

    public:
        void join() override;

    public:
        int32_t getPriority() const;

    protected:
        int32_t m_priority;

        SDL_Thread * m_thread;

		SDL_mutex * m_processLock;

        SDL_cond * m_conditionVariable;
        SDL_mutex * m_conditionLock;

        ThreadTaskInterface * m_task;

        AtomicBool m_exit;

#ifndef NDEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadIdentity> SDLThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
