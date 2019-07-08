#pragma once

#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadConditionVariableInterface.h"

#include "ThreadQueue.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ThreadMutexDummy.h"
#include "Kernel/Factory.h"

#include "Config/String.h"
#include "Config/Vector.h"

namespace Mengine
{
    class ThreadService
        : public ServiceBase<ThreadServiceInterface>
    {
    public:
        ThreadService();
        ~ThreadService() override;

    public:
        bool _availableService() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ThreadJobPtr createJob( uint32_t _sleep, const Char * _doc ) override;

    public:
        bool createThread( const ConstString & _threadName, int32_t _priority, const Char * _doc ) override;
        bool destroyThread( const ConstString & _threadName ) override;

    public:
        bool hasThread( const ConstString & _name ) const override;

    public:
        bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task ) override;
        bool joinTask( const ThreadTaskInterfacePtr & _task ) override;

    public:
        void stopTasks() override;

    public:
        ThreadQueueInterfacePtr createTaskQueue( uint32_t _packetSize, const Char * _doc ) override;
        void cancelTaskQueue( const ThreadQueueInterfacePtr & _queue ) override;

    public:
        void waitMainThreadCode( const LambdaMainThreadCode & _lambda, const Char * _doc ) override;

    public:
        void update() override;

    public:
        ThreadMutexInterfacePtr createMutex( const Char * _doc ) override;

    public:
        void sleep( uint32_t _ms ) override;

    public:
        ptrdiff_t getCurrentThreadId() override;
        bool isMainThread() const override;

    protected:
        uint32_t m_threadCount;

        struct ThreadTaskDesc
        {
            ThreadTaskInterfacePtr task;
            ThreadIdentityInterfacePtr identity;
            ConstString threadName;
            bool progress;
            bool complete;
        };

        typedef Vector<ThreadTaskDesc> VectorThreadTaskDesc;
        VectorThreadTaskDesc m_tasks;

        typedef Vector<ThreadQueuePtr> VectorThreadQueues;
        VectorThreadQueues m_threadQueues;

        FactoryPtr m_factoryThreadQueue;
        FactoryPtr m_factoryThreadJob;
        FactoryPtr m_factoryThreadMutexDummy;

        struct ThreadDesc
        {
            ConstString name;
            ThreadIdentityInterfacePtr identity;
        };

        typedef Vector<ThreadDesc> VectorThreadDescs;
        VectorThreadDescs m_threads;

        ThreadMutexInterfacePtr m_mutexMainCode;

        struct MainCodeDesc
        {
            ThreadConditionVariableInterfacePtr conditionVariable;
            LambdaMainThreadCode lambda;
#ifdef MENGINE_DEBUG
            String doc;
#endif
        };

        typedef Vector<MainCodeDesc> VectorMainCodeDescs;
        VectorMainCodeDescs m_mainCodes;

        ptrdiff_t m_mainThreadId;

    protected:
        void tryFastProcessTask_( ThreadTaskDesc & _desc );
    };
}
