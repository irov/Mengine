#pragma once

#include "Interface/ThreadIdentityInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        const ConstString & getCurrentThreadName();

        typedef Lambda<void()> LambdaDispatchMainThreadEvent;
        void dispatchMainThreadEvent( const LambdaDispatchMainThreadEvent & _event );

        ThreadIdentityInterfacePtr createThreadIdentity( const ThreadDescription & _description, EThreadPriority _priority, const LambdaThreadRunner & _lambda, uint32_t _sleep, const DocumentInterfacePtr & _doc );
    }
}