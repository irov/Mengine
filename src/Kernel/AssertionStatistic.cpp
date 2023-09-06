#include "AssertionStatistic.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/StatisticHelper.h"
#   include "Kernel/StatisticName.h"
#   include "Kernel/Assertion.h"
#   include "Kernel/Logger.h"

#   include "Config/StdIO.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionStatisticEmpty( const Char * _category, uint32_t _id, const Char * _file, uint32_t _line )
        {
            int64_t value = STATISTIC_GET_INTEGER( _id );

            if( value == 0 )
            {
                return;
            }

            Char msg[1024] = {'\0'};
            MENGINE_SNPRINTF( msg, 1024, "Statistics '%s' not empty ['%" MENGINE_PRLPARAM "']"
                , Helper::getStatisticName( _id )
                , value
            );

            Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Staticstic Empty" );
        }
    }
}
#endif