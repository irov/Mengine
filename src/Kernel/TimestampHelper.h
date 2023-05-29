#pragma once

#include "Config/Timestamp.h"

namespace Mengine
{
    namespace Helper
    {
        Timestamp getTimestamp();
        Timestamp getDurationTimestamp( Timestamp _timestamp );
    }
}