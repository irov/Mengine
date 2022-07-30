#pragma once

#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadConditionVariableInterface.h"

#include "ThreadQueue.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

#include "Kernel/String.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class ThreadService
        : public ServiceBase<ThreadServiceInterface>
    {
    public:
        ThreadService();
        ~ThreadService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ThreadJobPtr createJob( uint32_t _sleep, const DocumentPtr & _doc ) override;

    public:
        bool createThread( const ConstString & _threadName, EThreadPriority _priority, const DocumentPtr & _doc ) override;
        bool destroyThread( const ConstString & _threadName ) override;

    public:
        bool hasThread( const ConstString & _threadName ) const override;

    public:
        bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task, const DocumentPtr & _doc ) override;
        bool joinTask( const ThreadTaskInterfacePtr & _task ) override;

    public:
        void stopTasks() override;

    public:
        ThreadQueueInterfacePtr createTaskQueue( uint32_t _packetSize, const DocumentPtr & _doc ) override;
        void cancelTaskQueue( const ThreadQueueInterfacePtr & _queue ) override;

    public:
        void update() override;

    public:
        bool isMainThread() const override;
        uint64_t getMainThreadId() const override;

    public:
        const ConstString & getCurrentThreadName() const override;
        const ConstString & findThreadNameById( uint64_t _id ) const override;

    protected:
        uint32_t m_threadCount;

        ThreadMutexInterfacePtr m_mutex;

        struct ThreadTaskDesc
        {
            ThreadTaskInterfacePtr task;
            ThreadIdentityInterfacePtr identity;
            ConstString threadName;
            bool progress;
            bool complete;

#if MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<ThreadTaskDesc> VectorThreadTaskDesc;
        VectorThreadTaskDesc m_tasks;

        typedef Vector<ThreadQueuePtr> VectorThreadQueues;
        VectorThreadQueues m_threadQueues;

        FactoryPtr m_factoryThreadQueue;
        FactoryPtr m_factoryThreadJob;

        struct ThreadDesc
        {
            ConstString name;
            ThreadIdentityInterfacePtr identity;
        };

        typedef Vector<ThreadDesc> VectorThreadDescs;
        VectorThreadDescs m_threads;

        uint64_t m_mainThreadId;

    protected:
        void tryFastProcessTask_( ThreadTaskDesc & _desc );
    };
}
