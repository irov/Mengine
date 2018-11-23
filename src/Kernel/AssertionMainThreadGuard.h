#pragma once

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Assertion.h"

#ifndef NDEBUG
#   define MENGINE_ASSERTION_MAIN_THREAD_GUARD() MENGINE_ASSERTION( THREAD_SERVICE()->isMainThread(), ("This method use not main thread") )
#else
#   define MENGINE_ASSERTION_MAIN_THREAD_GUARD()
#endif