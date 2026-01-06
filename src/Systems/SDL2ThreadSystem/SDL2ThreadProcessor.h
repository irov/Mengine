#pragma once

#include "Interface/ThreadProcessorInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2ThreadProcessor
        : public ThreadProcessorInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL2ThreadProcessor );

    public:
        SDL2ThreadProcessor();
        ~SDL2ThreadProcessor() override;

    public:
        bool initialize( const ThreadDescription & _description, EThreadPriority _priority, const ThreadMutexInterfacePtr & _mutex );
        void finalize();

    public:
        void main();

    public:
        ThreadId getThreadId() const override;

    public:
        bool processTask( const ThreadTaskInterfacePtr & _task ) override;
        void removeTask() override;

    public:
        void join() override;

    public:
        bool isCurrentThread() const override;

    public:
        EThreadPriority getPriority() const;
        const ThreadDescription & getDescription() const override;

    protected:
        EThreadPriority m_priority;
        ThreadDescription m_description;

        SDL_threadID m_threadId;

        ThreadMutexInterfacePtr m_mutex;

        SDL_Thread * m_thread;

        SDL_mutex * m_taskLock;
        SDL_mutex * m_processLock;

        SDL_cond * m_conditionVariable;
        SDL_mutex * m_conditionLock;

        ThreadTaskInterfacePtr m_task;

        AtomicBool m_exit;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL2ThreadProcessor, ThreadProcessorInterface> SDL2ThreadProcessorPtr;
    //////////////////////////////////////////////////////////////////////////
}
