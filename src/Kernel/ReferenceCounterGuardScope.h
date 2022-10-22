#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

namespace Mengine
{
    class ReferenceCounterGuardScope
    {
    public:
        ReferenceCounterGuardScope( Factorable * _factorable, const Char * _format, ... );
        ~ReferenceCounterGuardScope();

    protected:
        FactorablePtr m_factorable;

        String m_message;
    };
}

#ifdef MENGINE_DEBUG
#   define REFERENCECOUNTER_GUARD_SCOPE( Timeout, ... ) Mengine::ReferenceCounterGuardScope rcgs__##__LINE__( Timeout, __VA_ARGS__ )
#else
#   define REFERENCECOUNTER_GUARD_SCOPE( Timeout, ... )
#endif