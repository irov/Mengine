#pragma once

#include "Interface/ThreadTaskInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ThreadEnum.h"

#include "Config/Thread.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadProcessorInterface
        : public Mixin
    {
    public:
        virtual ThreadId getThreadId() const = 0;

    public:
        virtual bool processTask( ThreadTaskInterface * _task ) = 0;
        virtual void removeTask() = 0;

    public:
        virtual void join() = 0;

    public:
        virtual bool isCurrentThread() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadProcessorInterface> ThreadProcessorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}