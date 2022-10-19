#pragma once

#include "Kernel/ThreadTask.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadTaskPacket
        : public ThreadTask
    {
        DECLARE_FACTORABLE( ThreadTaskPacket );

    public:
        ThreadTaskPacket();
        ~ThreadTaskPacket() override;

    public:
        bool initialize( uint32_t _packetSize );

    public:
        void addTask( const ThreadTaskPtr & _task );
        const ThreadTaskPtr & getTask( uint32_t _index ) const;
        uint32_t countTask() const;

    protected:
        bool _onThreadTaskProcess() override;

    protected:
        bool _onThreadTaskRun() override;
        void _onThreadTaskCancel() override;
        void _onThreadTaskUpdate() override;
        void _onThreadTaskJoin() override;

    protected:
        void _onThreadTaskFinish() override;
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        typedef Vector<ThreadTaskPtr> VectorThreadTasks;
        VectorThreadTasks m_tasks;

        ThreadMutexInterfacePtr m_childMutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPacket> ThreadTaskPacketPtr;
    //////////////////////////////////////////////////////////////////////////
}