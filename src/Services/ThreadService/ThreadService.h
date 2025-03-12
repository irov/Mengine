#pragma once

#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadProcessorInterface.h"
#include "Interface/ThreadConditionVariableInterface.h"
#include "Interface/FactoryInterface.h"

#include "ThreadQueue.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Config/ThreadId.h"

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
        ThreadJobPtr createJob( uint32_t _sleep, const DocumentInterfacePtr & _doc ) override;

    public:
        bool createThreadProcessor( const ConstString & _processorName, const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) override;
        bool destroyThreadProcessor( const ConstString & _processorName ) override;

    public:
        bool hasThreadProcessor( const ConstString & _processorName ) const override;

    public:
        void dispatchMainThreadEvent( const LambdaEvent & _event ) override;

    public:
        bool addTask( const ConstString & _processorName, const ThreadTaskInterfacePtr & _task, const DocumentInterfacePtr & _doc ) override;
        bool joinTask( const ThreadTaskInterfacePtr & _task ) override;

    public:
        void stopTasks() override;

    public:
        ThreadQueueInterfacePtr createTaskQueue( uint32_t _packetSize, const DocumentInterfacePtr & _doc ) override;
        void cancelTaskQueue( const ThreadQueueInterfacePtr & _queue ) override;

    public:
        void update() override;

    public:
        void updateMainThread() override;
        bool isMainThread() const override;
        ThreadId getMainThreadId() const override;

    public:
        const ConstString & getCurrentThreadName() const override;
        const ConstString & findThreadNameById( ThreadId _id ) const override;

    protected:
        ThreadMutexInterfacePtr m_mutexTasks;
        ThreadMutexInterfacePtr m_mutexThreads;
        ThreadMutexInterfacePtr m_mutexDispatchEvents;

        typedef Vector<LambdaEvent> VectorEvents;
        VectorEvents m_dispatchEvents;
        VectorEvents m_dispatchEventsAux;

        struct ThreadTaskDesc
        {
            ThreadTaskInterfacePtr task;
            ThreadProcessorInterfacePtr processor;
            ConstString processorName;
            bool progress;
            bool complete;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
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

        ThreadId m_mainThreadId;

    protected:
        void tryFastProcessTask_( ThreadTaskDesc & _desc );
    };
}
