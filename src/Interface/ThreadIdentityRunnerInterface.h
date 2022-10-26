#pragma once

#include "Kernel/Mixin.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadIdentityRunnerInterface
        : public Mixin
    {
    public:
        virtual void run() = 0;
        virtual void cancel() = 0;

    public:
        virtual bool isCancel() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadIdentityRunnerInterface> ThreadIdentityRunnerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const ThreadIdentityRunnerInterfacePtr & )> LambdaThreadRunner;
    //////////////////////////////////////////////////////////////////////////
}