#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Interface/ThreadServiceInterface.h"

#   define MENGINE_ASSERTION_MAIN_THREAD_GUARD() MENGINE_ASSERTION( THREAD_SERVICE()->isMainThread(), "This method use not main thread" )
#else
#   define MENGINE_ASSERTION_MAIN_THREAD_GUARD()
#endif