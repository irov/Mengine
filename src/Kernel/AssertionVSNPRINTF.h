#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Config/Char.h"
#   include "Config/StdArg.h"
#   include "Config/StdInt.h"
#   include "Config/StdDef.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionVSNPRINTF( const Char * _category, size_t _capacity, const Char * _format, MENGINE_VA_LIST_TYPE _args, const Char * _file, int32_t _line, const Char * _function );
    }
}

#   define MENGINE_ASSERTION_VSNPRINTF(CAPACITY, FORMAT, ARGS) Mengine::Detail::assertionVSNPRINTF(MENGINE_CODE_LIBRARY, CAPACITY, FORMAT, ARGS, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#else
#   define MENGINE_ASSERTION_VSNPRINTF(CAPACITY, FORMAT, ARGS)
#endif
