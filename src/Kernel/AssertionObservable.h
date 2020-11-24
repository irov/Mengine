#pragma once

#include "Kernel/Observable.h"

#include "Config/Config.h"

namespace Mengine
{
    namespace Helper
    {
        bool assertionHasObserver( Observable * _observer );
    }
}

#if MENGINE_ASSERTION_DEBUG
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_OBSERVABLE(Ptr, ...) if( Mengine::Helper::assertionHasObserver( Ptr ) == true ) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_FATAL, "observer", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_OBSERVABLE(Ptr, ...)
#endif
