#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadQueueInterface.h"

#include "Kernel/ThreadEnum.h"
#include "Kernel/ThreadJob.h"
#include "Kernel/ConstString.h"
#include "Config/Lambda.h"

namespace Mengine
{
    class ThreadServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ThreadService" )

    public:
        virtual void update() = 0;

    public:
        virtual ThreadJobPtr createJob( uint32_t _sleep, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool createThreadProcessor( const ConstString & _threadName, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool destroyThreadProcessor( const ConstString & _threadName ) = 0;

    public:
        virtual bool hasThreadProcessor( const ConstString & _threadName ) const = 0;

    public:
        virtual bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool joinTask( const ThreadTaskInterfacePtr & _task ) = 0;

    public:
        virtual void stopTasks() = 0;

    public:
        virtual ThreadQueueInterfacePtr createTaskQueue( uint32_t _packetSize, const DocumentInterfacePtr & _doc ) = 0;
        virtual void cancelTaskQueue( const ThreadQueueInterfacePtr & _queue ) = 0;

    public:
        virtual bool isMainThread() const = 0;
        virtual ThreadId getMainThreadId() const = 0;

    public:
        virtual const ConstString & getCurrentThreadName() const = 0;
        virtual const ConstString & findThreadNameById( ThreadId _id ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define THREAD_SERVICE()\
    ((Mengine::ThreadServiceInterface*)SERVICE_GET(Mengine::ThreadServiceInterface))
//////////////////////////////////////////////////////////////////////////