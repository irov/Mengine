#pragma once

#include "ThreadTask.h"

#include "Config/Vector.h"

namespace Mengine
{
    class ThreadTaskPacket
        : public ThreadTask
    {
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
        bool _onMain() override;

    protected:
        bool _onRun() override;
        void _onCancel() override;
        void _onUpdate() override;
        void _onJoin() override;

    protected:
        void _onComplete( bool _successful ) override;

    protected:
        typedef Vector<ThreadTaskPtr> VectorThreadTasks;
        VectorThreadTasks m_tasks;

        ThreadMutexInterfacePtr m_childMutex;
    };

    typedef IntrusivePtr<ThreadTaskPacket> ThreadTaskPacketPtr;
}