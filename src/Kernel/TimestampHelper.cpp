#include "TimestampHelper.h"

#include "Interface/TimeSystemInterface.h"
#include "Interface/DateTimeSystemInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        Timestamp getSystemTimestamp()
        {
            Timestamp time = TIME_SYSTEM()
                ->getSystemTimestamp();

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
        Timestamp getLocalTimestamp()
        {
            Timestamp systemTime = TIME_SYSTEM()
                ->getSystemTimestamp();

            int64_t offset = DATETIME_SYSTEM()
                ->getTimeZoneOffsetMilliseconds();

            Timestamp localTime = systemTime - offset;

            return localTime;
        }
        //////////////////////////////////////////////////////////////////////////
        Timestamp getSystemDurationTimestamp( Timestamp _timestamp )
        {
            Timestamp current_timestamp = Helper::getSystemTimestamp();

            Timestamp duration_timestamp = current_timestamp - _timestamp;

            return duration_timestamp;
        }
        //////////////////////////////////////////////////////////////////////////
        Timestamp getLocalDurationTimestamp( Timestamp _timestamp )
        {
            Timestamp current_timestamp = Helper::getLocalTimestamp();

            Timestamp duration_timestamp = current_timestamp - _timestamp;

            return duration_timestamp;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}