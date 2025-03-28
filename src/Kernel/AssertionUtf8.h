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
        void assertionValidateUtf8( const Char * _category, const Utf8 * _value, size_t _len, const Char * _file, int32_t _line, const Char * _function );
    }
}

#   define MENGINE_ASSERTION_VALIDATE_UTF8(Value, Len) Mengine::Detail::assertionValidateUtf8(MENGINE_CODE_LIBRARY, Value, Len, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#else
#   define MENGINE_ASSERTION_VALIDATE_UTF8(Value, Len)
#endif
