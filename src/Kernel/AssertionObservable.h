#pragma once

#include "Kernel/Observable.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"

namespace Mengine
{
    namespace Helper
    {
        bool assertionHasObserver( Observable * _observer );
    }
}

#   define MENGINE_ASSERTION_OBSERVABLE(Ptr, ...) if( Mengine::Helper::assertionHasObserver( Ptr ) == true ) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, "observer", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_OBSERVABLE(Ptr, ...)
#endif