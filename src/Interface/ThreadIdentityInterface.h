#pragma once

#include "Interface/ThreadIdentityRunnerInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ThreadEnum.h"

#include "Config/Lambda.h"
#include "Config/Thread.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadIdentityInterface
        : public Mixin
    {
    public:
        virtual ThreadId getThreadId() const = 0;

    public:        
        virtual ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual void join() = 0;

    public:
        virtual bool isCurrentThread() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadIdentityInterface> ThreadIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}