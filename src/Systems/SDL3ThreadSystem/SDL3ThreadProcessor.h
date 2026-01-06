#pragma once

#include "Interface/ThreadProcessorInterface.h"

#include "Environment/SDL3/SDL3Includer.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL3ThreadProcessor
        : public ThreadProcessorInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL3ThreadProcessor );

    public:
        SDL3ThreadProcessor();
        ~SDL3ThreadProcessor() override;

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

        SDL_ThreadID m_threadId;

        ThreadMutexInterfacePtr m_mutex;

        SDL_Thread * m_thread;

        SDL_Mutex * m_taskLock;
        SDL_Mutex * m_processLock;

        SDL_Condition * m_conditionVariable;
        SDL_Mutex * m_conditionLock;

        ThreadTaskInterfacePtr m_task;

        AtomicBool m_exit;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL3ThreadProcessor, ThreadProcessorInterface> SDL3ThreadProcessorPtr;
    //////////////////////////////////////////////////////////////////////////
}



