#pragma once

#include "Config/Timestamp.h"

namespace Mengine
{
    namespace Helper
    {
        double getElapsedTime();

        Timestamp getSystemTimestamp();
        Timestamp getLocalTimestamp();

        Timestamp getSystemDurationTimestamp( Timestamp _timestamp );
        Timestamp getLocalDurationTimestamp( Timestamp _timestamp );
    }
}