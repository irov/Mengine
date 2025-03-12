#pragma once

#include "Interface/ThreadIdentityRunnerInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ThreadEnum.h"
#include "Kernel/ConstString.h"
#include "Kernel/ThreadDescription.h"

#include "Config/Lambda.h"
#include "Config/ThreadId.h"
#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadIdentityInterface
        : public Mixin
    {
    public:
        virtual EThreadPriority getPriority() const = 0;
        virtual const ThreadDescription & getDescription() const = 0;

    public:
        virtual ThreadId getThreadId() const = 0;

    public:
        virtual ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda, Timestamp _sleep, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual const ThreadIdentityRunnerInterfacePtr & getRunner() const = 0;

    public:
        virtual void join( bool _cancel ) = 0;

    public:
        virtual bool isCurrentThread() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadIdentityInterface> ThreadIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
