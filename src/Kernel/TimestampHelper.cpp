#include "TimestampHelper.h"

#include "Interface/TimeSystemInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        Timestamp getTimestamp()
        {
            Timestamp time = TIME_SYSTEM()
                ->getTimestamp();

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
        Timestamp getDurationTimestamp( Timestamp _timestamp )
        {
            Timestamp current_timestamp = Helper::getTimestamp();

            Timestamp duration_timestamp = current_timestamp - _timestamp;

            return duration_timestamp;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}