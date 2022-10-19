#pragma once

#include "Config/Time.h"
#include "Config/Char.h"
#include "Config/Atomic.h"

#include <thread>

namespace Mengine
{
    class TimeoutGuardScope
    {
    public:
        TimeoutGuardScope( TimeMilliseconds _timeout, const Char * _format, ... );
        ~TimeoutGuardScope();

    public:
        std::thread m_thread;

        AtomicBool * m_progress;
    };
}

#ifdef MENGINE_DEBUG
#   define TIMEOUT_GUARD_SCOPE( Timeout, ... ) TimeoutGuardScope tgs__##__LINE__( Timeout, __VA_ARGS__ )
#else
#   define TIMEOUT_GUARD_SCOPE( Timeout, ... )
#endif