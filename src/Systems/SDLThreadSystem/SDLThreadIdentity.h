#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

#include "Config/String.h"
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
        bool initialize( int32_t _priority, const ConstString & _name, const ThreadMutexInterfacePtr & _mutex, const DocumentPtr & _doc );

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
        ConstString m_name;

        ThreadMutexInterfacePtr m_mutex;

        SDL_Thread * m_thread;

        SDL_mutex * m_taskLock;
        SDL_mutex * m_processLock;

        SDL_cond * m_conditionVariable;
        SDL_mutex * m_conditionLock;

        ThreadTaskInterface * m_task;

        AtomicBool m_exit;

#ifdef MENGINE_DEBUG
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadIdentity> SDLThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
