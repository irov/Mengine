#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Config/Char.h"
#   include "Config/Typedef.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionStatisticEmpty( const Char * _category, uint32_t _id, const Char * _file, uint32_t _line );
    }
}

#   define MENGINE_ASSERTION_STATISTIC_EMPTY(ID) Detail::assertionStatisticEmpty(MENGINE_CODE_LIBRARY, ID, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#else
#   define MENGINE_ASSERTION_STATISTIC_EMPTY(ID)
#endif