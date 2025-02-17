#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Config/Char.h"
#   include "Config/Utf8.h"
#   include "Config/Typedef.h"

#   include "Kernel/Identity.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionIdentity( const Char * _category, const Identity * _identity, const Char * _file, uint32_t _line, const Char * _function );
    }
}

#   define MENGINE_ASSERTION_IDENTITY(Identity) Mengine::Detail::assertionIdentity(MENGINE_CODE_LIBRARY, Identity, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#else
#   define MENGINE_ASSERTION_IDENTITY(Identity)
#endif