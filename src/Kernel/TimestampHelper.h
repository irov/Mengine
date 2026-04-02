#pragma once

#include "Config/Timestamp.h"

namespace Mengine
{
    namespace Helper
    {
        double getElapsedTime();

        Timestamp getSystemTimestamp();
        Timestamp getPlatformTimestamp();
        Timestamp getBootTimestamp();

        Timestamp convertTimestampTimebase( Timestamp _timestamp, Timestamp _currentSourceTimestamp, Timestamp _currentTargetTimestamp );
        Timestamp convertTimestampNanosecondsToMilliseconds( Timestamp _timestampNanoseconds );

        Timestamp getLocalTimestamp();

        Timestamp getSystemDurationTimestamp( Timestamp _timestamp );
        Timestamp getLocalDurationTimestamp( Timestamp _timestamp );
    }
}