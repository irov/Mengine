#pragma once

#include "Kernel/LoggerMessage.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class ReferenceCounterGuardScope
    {
    public:
        ReferenceCounterGuardScope( Factorable * _factorable, const Char * _format, ... );
        ~ReferenceCounterGuardScope();

    protected:
        FactorablePtr m_factorable;

        Char m_message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
    };
}

#if defined(MENGINE_DEBUG)
#   define REFERENCECOUNTER_GUARD_SCOPE( Timeout, ... ) Mengine::ReferenceCounterGuardScope MENGINE_PP_CONCATENATE(rcgs__, MENGINE_CODE_COUNTER)( Timeout, __VA_ARGS__ )
#else
#   define REFERENCECOUNTER_GUARD_SCOPE( Timeout, ... )
#endif