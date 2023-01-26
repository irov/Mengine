#pragma once

#include "Config/Config.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#include "Config/Char.h"
#include "Config/StdArg.h"
#include "Config/StdInt.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        void assertionVSNPRINTF( size_t _capacity, const Char * _format, MENGINE_VA_LIST_TYPE _args, const Char * _file, uint32_t _line );
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_ASSERTION_VSNPRINTF(CAPACITY, FORMAT, ARGS) Detail::assertionVSNPRINTF(CAPACITY, FORMAT, ARGS, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#else
#   define MENGINE_ASSERTION_VSNPRINTF(CAPACITY, FORMAT, ARGS)
#endif
