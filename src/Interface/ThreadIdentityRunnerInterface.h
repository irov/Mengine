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
        virtual bool run() = 0;
        virtual void cancel() = 0;

    public:
        virtual void sleep() = 0;

    public:
        virtual bool isCancel() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadIdentityRunnerInterface> ThreadIdentityRunnerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const ThreadIdentityRunnerInterfacePtr & )> LambdaThreadRunner;
    //////////////////////////////////////////////////////////////////////////
}