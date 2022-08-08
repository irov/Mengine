#pragma once

#include "Config/Config.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#   include "Interface/ThreadServiceInterface.h"

#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_MAIN_THREAD_GUARD() MENGINE_ASSERTION( THREAD_SERVICE()->isMainThread(), "This method use not main thread" )
#else
#   define MENGINE_ASSERTION_MAIN_THREAD_GUARD()
#endif