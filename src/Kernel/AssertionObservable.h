#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Observable.h"

namespace Mengine
{
    namespace Helper
    {
        bool assertionHasObserver( Observable * _observer );
    }
}

#   define MENGINE_ASSERTION_OBSERVABLE(Ptr, ...) if( Mengine::Helper::assertionHasObserver( Ptr ) == true ) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, "observer", MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION ) DETAIL__MENGINE_ASSERTION_CALL(__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_OBSERVABLE(Ptr, ...)
#endif