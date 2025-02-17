#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Config/Char.h"
#   include "Config/Typedef.h"
#   include "Config/StatisticId.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionStatisticEmpty( const Char * _category, StatisticId _id, const Char * _file, uint32_t _line, const Char * _function );
    }
}

#   define MENGINE_ASSERTION_STATISTIC_EMPTY(ID) Mengine::Detail::assertionStatisticEmpty(MENGINE_CODE_LIBRARY, ID, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#else
#   define MENGINE_ASSERTION_STATISTIC_EMPTY(ID)
#endif