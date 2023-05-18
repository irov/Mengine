#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/ConstString.h"

#   include "Config/Char.h"
#   include "Config/Typedef.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionStandardCharacterSet( const Char * _category, const Char * _value, size_t _len, const Char * _file, uint32_t _line );
        void assertionLowerCharacterSet( const Char * _category, const Char * _value, size_t _len, const Char * _file, uint32_t _line );
        void assertionUpperCharacterSet( const Char * _category, const Char * _value, size_t _len, const Char * _file, uint32_t _line );
    }
}

#   define MENGINE_ASSERTION_STANDARD_CHARACTER_SET(Value, Len) Detail::assertionStandardCharacterSet(MENGINE_CODE_LIBRARY, Value, Len, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#   define MENGINE_ASSERTION_LOWER_CHARACTER_SET(Value, Len) Detail::assertionLowerCharacterSet(MENGINE_CODE_LIBRARY, Value, Len, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#   define MENGINE_ASSERTION_UPPER_CHARACTER_SET(Value, Len) Detail::assertionUpperCharacterSet(MENGINE_CODE_LIBRARY, Value, Len, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#else
#   define MENGINE_ASSERTION_STANDARD_CHARACTER_SET(Value, Len)
#   define MENGINE_ASSERTION_LOWER_CHARACTER_SET(Value, Len)
#   define MENGINE_ASSERTION_UPPER_CHARACTER_SET(Value, Len)
#endif