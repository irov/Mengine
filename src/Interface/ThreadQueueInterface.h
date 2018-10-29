#pragma once

#include "Interface/ThreadTaskInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
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
}