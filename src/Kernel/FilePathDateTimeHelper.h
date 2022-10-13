#pragma once

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t makeLoggerDateTimestamp( Char * const _timestamp, size_t _capacity );
        size_t makeFilePathDateTimestamp( Char * const _timestamp, size_t _capacity );
    }
}