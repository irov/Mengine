#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Mixin.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadTaskInterface
        : public Mixin
    {
    public:
        virtual bool isRun() const = 0;
        virtual bool isComplete() const = 0;
        virtual bool isSuccessful() const = 0;
        virtual bool isCancel() const = 0;

    public:
        virtual void preparation() = 0;
        virtual void main() = 0;
        virtual void finally() = 0;

    public:
        virtual bool run( const ThreadMutexInterfacePtr & _mutex ) = 0;
        virtual bool cancel() = 0;
        virtual bool update() = 0;

    public:
        virtual void join() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskInterface> ThreadTaskInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}