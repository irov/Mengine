#pragma once

#include "Interface/ThreadProcessorInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Atomic.h"

#include <pthread.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class POSIXThreadProcessor
        : public ThreadProcessorInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( POSIXThreadProcessor );

    public:
        POSIXThreadProcessor();
        ~POSIXThreadProcessor() override;

    public:
        bool initialize( const ThreadDescription & _description, EThreadPriority _priority, const ThreadMutexInterfacePtr & _mutex );
        void finalize();

    public:
        void main();

    public:
        EThreadPriority getPriority() const override;
        const ThreadDescription & getDescription() const override;
        
    public:
        ThreadId getThreadId() const override;

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        void removeTask() override;

    public:
        void join() override;

    public:
        bool isCurrentThread() const override;
        
    protected:
        ThreadDescription m_description;
        EThreadPriority m_priority;

        ThreadMutexInterfacePtr m_mutex;

        pthread_t m_threadId;

        pthread_mutex_t m_taskLock;
        pthread_mutex_t m_processLock;

        pthread_cond_t m_conditionVariable;
        pthread_mutex_t m_conditionLock;

        ThreadTaskInterface * m_task;

        AtomicBool m_exit;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<POSIXThreadProcessor, ThreadProcessorInterface> POSIXThreadProcessorPtr;
    //////////////////////////////////////////////////////////////////////////
}
