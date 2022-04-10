#include "TimeHelper.h"

#include "Interface/TimeSystemInterface.h"

namespace Mengine
{
    namespace Helper
    {
        TimeMilliseconds getTimeMilliseconds()
        {
            TimeMilliseconds time = TIME_SYSTEM()
                ->getTimeMilliseconds();

            return time;
        }
    }
}