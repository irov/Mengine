#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Config/Char.h"
#   include "Config/Utf8.h"
#   include "Config/Typedef.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionValidateUtf8( const Char * _category, const Utf8 * _value, size_t _len, const Char * _file, uint32_t _line );
    }
}

#   define MENGINE_ASSERTION_VALIDATE_UTF8(Value, Len) Detail::assertionValidateUtf8(MENGINE_CODE_LIBRARY, Value, Len, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#else
#   define MENGINE_ASSERTION_VALIDATE_UTF8(Value, Len)
#endif