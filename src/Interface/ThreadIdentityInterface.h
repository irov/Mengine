#pragma once

#include "Interface/ThreadTaskInterface.h"

#include "Kernel/Mixin.h"

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