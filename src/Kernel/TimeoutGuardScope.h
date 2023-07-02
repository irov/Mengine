#pragma once

#include "Interface/ThreadIdentityInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Timestamp.h"
#include "Config/Char.h"
#include "Config/Atomic.h"

namespace Mengine
{
    class TimeoutGuardScope
    {
    public:
        TimeoutGuardScope( Timestamp _timeout, const DocumentInterfacePtr & _doc, const Char * _format, ... );
        ~TimeoutGuardScope();

    public:
        ThreadIdentityInterfacePtr m_threadIdentity;
    };
}

#if defined(MENGINE_DEBUG)
#   define TIMEOUT_GUARD_SCOPE( Timeout, Doc, ... ) Mengine::TimeoutGuardScope tgs__##__LINE__( Timeout, Doc, __VA_ARGS__ )
#else
#   define TIMEOUT_GUARD_SCOPE( Timeout, Doc, ... )
#endif