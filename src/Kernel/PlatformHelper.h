#pragma once

#include "Kernel/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        typedef Lambda<void()> LambdaDispatchMainThreadEvent;

        void dispatchMainThreadEvent( const LambdaDispatchMainThreadEvent & _event );
    }
}