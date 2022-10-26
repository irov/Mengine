#pragma once

#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadProcessorInterface.h"
#include "Interface/ThreadConditionVariableInterface.h"
#include "Interface/FactoryInterface.h"

#include "ThreadQueue.h"

#include "Kernel/ServiceBase.h"
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
        bool createThreadProcessor( const ConstString & _threadName, EThreadPriority _priority, const DocumentPtr & _doc ) override;
        bool destroyThreadProcessor( const ConstString & _threadName ) override;

    public:
        ThreadIdentityInterfacePtr createThreadIdentity( const ConstString & _threadName, EThreadPriority _priority, const DocumentPtr & _doc ) override;
        bool destroyThreadIdentity( const ThreadIdentityInterfacePtr & _threadIdentity ) override;

    public:
        bool hasThreadProcessor( const ConstString & _threadName ) const override;

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
            ThreadProcessorInterfacePtr processor;
            ConstString threadName;
            bool progress;
            bool complete;

#ifdef MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<ThreadTaskDesc> VectorThreadTaskDesc;
        VectorThreadTaskDesc m_tasks;

        typedef Vector<ThreadTaskInterfacePtr> VectorThreadTasks;
        VectorThreadTasks m_tasksAux;

        typedef Vector<ThreadQueuePtr> VectorThreadQueues;
        VectorThreadQueues m_threadQueues;

        FactoryInterfacePtr m_factoryThreadQueue;
        FactoryInterfacePtr m_factoryThreadJob;

        struct ThreadProcessorDesc
        {
            ConstString name;
            ThreadProcessorInterfacePtr processor;
        };

        typedef Vector<ThreadProcessorDesc> VectorThreadProcessorDescs;
        VectorThreadProcessorDescs m_threadProcessors;

        typedef Vector<ThreadIdentityInterfacePtr> VectorThreadIdentityDescs;
        VectorThreadIdentityDescs m_threadIdentities;

        uint64_t m_mainThreadId;

    protected:
        void tryFastProcessTask_( ThreadTaskDesc & _desc );
    };
}
