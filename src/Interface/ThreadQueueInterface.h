#pragma once

#include "Interface/ThreadTaskInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadQueueInterface
        : public Mixin
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void addThread( const ConstString & _threadName ) = 0;

    public:
        virtual void addTask( const ThreadTaskInterfacePtr & _task ) = 0;

    public:
        virtual void cancel() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadQueueInterface> ThreadQueueInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}