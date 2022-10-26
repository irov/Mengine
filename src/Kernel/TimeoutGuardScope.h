#pragma once

#include "Interface/ThreadIdentityInterface.h"

#include "Kernel/Documentable.h"

#include "Config/Time.h"
#include "Config/Char.h"
#include "Config/Atomic.h"

namespace Mengine
{
    class TimeoutGuardScope
    {
    public:
        TimeoutGuardScope( TimeMilliseconds _timeout, const DocumentablePtr & _doc, const Char * _format, ... );
        ~TimeoutGuardScope();

    public:
        ThreadIdentityInterfacePtr m_threadIdentity;
    };
}

#ifdef MENGINE_DEBUG
#   define TIMEOUT_GUARD_SCOPE( Timeout, Doc, ... ) Mengine::TimeoutGuardScope tgs__##__LINE__( Timeout, Doc, __VA_ARGS__ )
#else
#   define TIMEOUT_GUARD_SCOPE( Timeout, Doc, ... )
#endif