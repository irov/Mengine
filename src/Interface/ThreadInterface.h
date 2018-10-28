#pragma once

#include "Interface/ThreadTaskInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const int32_t MENGINE_THREAD_PRIORITY_LOWEST = -2;
    const int32_t MENGINE_THREAD_PRIORITY_BELOW_NORMAL = -1;
    const int32_t MENGINE_THREAD_PRIORITY_NORMAL = 0;
    const int32_t MENGINE_THREAD_PRIORITY_ABOVE_NORMAL = 1;
    const int32_t MENGINE_THREAD_PRIORITY_HIGHEST = 2;
    const int32_t MENGINE_THREAD_PRIORITY_TIME_CRITICAL = 3;
    //////////////////////////////////////////////////////////////////////////
    class ThreadQueueInterface
        : public Mixin
    {
    public:
        virtual void addTask( const ThreadTaskInterfacePtr & _task ) = 0;

    public:
        virtual void cancel() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadQueueInterface> ThreadQueueInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class ThreadIdentityInterface
        : public Mixin
    {
    public:
        virtual bool processTask( ThreadTaskInterface * _task ) = 0;
        virtual bool completeTask() = 0;

    public:
        virtual void join() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadIdentityInterface> ThreadIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}