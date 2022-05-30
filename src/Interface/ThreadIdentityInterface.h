#pragma once

#include "Interface/ThreadTaskInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ThreadEnum.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadIdentityInterface
        : public Mixin
    {
    public:
        virtual uint64_t getThreadId() const = 0;

    public:
        virtual bool processTask( ThreadTaskInterface * _task ) = 0;
        virtual void removeTask() = 0;

    public:
        virtual void join() = 0;

    public:
        virtual bool isCurrentThread() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadIdentityInterface> ThreadIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}