#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ThreadQueueInterface.h"

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
        virtual ThreadJobPtr createJob( uint32_t _sleep ) = 0;

    public:
        virtual bool createThread( const ConstString & _threadName, int32_t _priority, const Char * _doc, uint32_t _line ) = 0;
        virtual bool destroyThread( const ConstString & _threadName ) = 0;

    public:
        virtual bool hasThread( const ConstString & _name ) const = 0;

    public:
        virtual bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task ) = 0;
        virtual bool joinTask( const ThreadTaskInterfacePtr & _task ) = 0;

    public:
        virtual void stopTasks() = 0;

    public:
        virtual ThreadQueueInterfacePtr runTaskQueue( uint32_t _packetSize ) = 0;
        virtual void cancelTaskQueue( const ThreadQueueInterfacePtr & _queue ) = 0;

    public:
        typedef Lambda<void()> LambdaMainCode;
        virtual void waitMainCode( const LambdaMainCode & _lambda, const Char * _file, uint32_t _line ) = 0;

    public:
        virtual ThreadMutexInterfacePtr createMutex( const Char * _file, uint32_t _line ) = 0;

    public:
        virtual void sleep( uint32_t _ms ) = 0;

    public:
        virtual ptrdiff_t getCurrentThreadId() = 0;
        virtual bool isMainThread() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define THREAD_SERVICE()\
    ((Mengine::ThreadServiceInterface*)SERVICE_GET(Mengine::ThreadServiceInterface))
//////////////////////////////////////////////////////////////////////////