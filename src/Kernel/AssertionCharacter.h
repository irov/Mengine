#pragma once

#include "Config/Config.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#include "Kernel/ConstString.h"

#include "Config/Char.h"
#include "Config/Typedef.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionStandardCharacterSet( const Char * _value, size_t _len, const Char * _file, uint32_t _line );
    }
}

#   define MENGINE_ASSERTION_STANDARD_CHARACTER_SET(Value, Len) Detail::assertionStandardCharacterSet(Value, Len, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#else
#   define MENGINE_ASSERTION_STANDARD_CHARACTER_SET(Value, Len)
#endif