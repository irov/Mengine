#pragma once

#include "Interface/ThreadQueueInterface.h"

#include "Kernel/ThreadTaskPacket.h"
#include "Kernel/ConstString.h"
#include "Kernel/Factory.h"

#include "Config/List.h"

namespace Mengine
{
    class ThreadQueue
        : public ThreadQueueInterface
        , public Factorable
    {
    public:
        ThreadQueue();
        ~ThreadQueue() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setPacketSize( uint32_t _size );

    public:
        bool update();

    protected:
        void addThread( const ConstString & _threadName ) override;

    protected:
        void addTask( const ThreadTaskInterfacePtr & _task ) override;

    protected:
        void cancel() override;

    protected:
        void updateCurrentTask_( ThreadTaskInterfacePtr & _currentTask );

    protected:
        uint32_t m_packetSize;
        uint32_t m_threadSampler;

        typedef Vector<ConstString> VectorThreads;
        VectorThreads m_threads;

        typedef List<ThreadTaskInterfacePtr> ListThreadTasks;
        ListThreadTasks m_threadTasks;

        typedef Vector<ThreadTaskInterfacePtr> VectorThreadTasks;
        VectorThreadTasks m_currentThreadTasks;

        FactoryPtr m_factoryPoolTaskPacket;

        bool m_cancel;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadQueue, ThreadQueueInterface> ThreadQueuePtr;
    //////////////////////////////////////////////////////////////////////////
}
