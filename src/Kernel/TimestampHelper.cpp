#include "TimestampHelper.h"

#include "Interface/TimeSystemInterface.h"
#include "Interface/DateTimeSystemInterface.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            MENGINE_CONSTEXPR Timestamp MENGINE_TIMESTAMP_NS_PER_MS = 1000000ULL;
        }
        //////////////////////////////////////////////////////////////////////////
        double getElapsedTime()
        {
            double time = TIME_SYSTEM()
                ->getElapsedTime();

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
        Timestamp getSystemTimestamp()
        {
            Timestamp time = TIME_SYSTEM()
                ->getSystemTimestamp();

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
        Timestamp getPlatformTimestamp()
        {
            Timestamp time = TIME_SYSTEM()
                ->getPlatformTimestamp();

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
        Timestamp getBootTimestamp()
        {
            Timestamp time = TIME_SYSTEM()
                ->getBootTimestamp();

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
        Timestamp convertTimestampTimebase( Timestamp _timestamp, Timestamp _currentSourceTimestamp, Timestamp _currentTargetTimestamp )
        {
            if( _currentSourceTimestamp >= _currentTargetTimestamp )
            {
                Timestamp offset = _currentSourceTimestamp - _currentTargetTimestamp;
                Timestamp timestamp = _timestamp >= offset
                    ? _timestamp - offset
                    : 0ULL;

                return timestamp;
            }

            Timestamp offset = _currentTargetTimestamp - _currentSourceTimestamp;
            Timestamp timestamp = _timestamp + offset;

            return timestamp;
        }
        //////////////////////////////////////////////////////////////////////////
        Timestamp convertTimestampNanosecondsToMilliseconds( Timestamp _timestampNanoseconds )
        {
            Timestamp timestamp = _timestampNanoseconds / Detail::MENGINE_TIMESTAMP_NS_PER_MS;

            return timestamp;
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