#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/ThreadHelper.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        const ConstString & getCurrentThreadName();

        typedef Lambda<void()> LambdaDispatchMainThreadEvent;
        void dispatchMainThreadEvent( const LambdaDispatchMainThreadEvent & _event );
    }
}