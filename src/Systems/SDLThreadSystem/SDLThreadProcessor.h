#pragma once

#include "Interface/ThreadProcessorInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Atomic.h"

#include "SDL_thread.h"
#include "SDL_timer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLThreadProcessor
        : public ThreadProcessorInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDLFileOutputStream );

    public:
        SDLThreadProcessor();
        ~SDLThreadProcessor() override;

    public:
        bool initialize( EThreadPriority _priority, const ConstString & _name, const ThreadMutexInterfacePtr & _mutex );
        void finalize();

    public:
        void main();

    public:
        ThreadId getThreadId() const override;

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        void removeTask() override;

    public:
        void join() override;

    public:
        bool isCurrentThread() const override;

    public:
        EThreadPriority getPriority() const;

    protected:
        EThreadPriority m_priority;
        ConstString m_name;

        SDL_threadID m_threadId;

        ThreadMutexInterfacePtr m_mutex;

        SDL_Thread * m_thread;

        SDL_mutex * m_taskLock;
        SDL_mutex * m_processLock;

        SDL_cond * m_conditionVariable;
        SDL_mutex * m_conditionLock;

        ThreadTaskInterface * m_task;

        AtomicBool m_exit;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadProcessor, ThreadProcessorInterface> SDLThreadProcessorPtr;
    //////////////////////////////////////////////////////////////////////////
}
