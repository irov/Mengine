#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        void makeLoggerDateTimestamp( Char * const _timestamp, size_t _capacity );
        void makeFilePathDateTimestamp( Char * const _timestamp, size_t _capacity );
    }
}
