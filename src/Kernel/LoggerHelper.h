#pragma once

#include "Interface/DateTimeProviderInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //format like "[%02u:%02u:%02u:%04u]"
        size_t makeLoggerTimestamp( const DateTimeProviderInterfacePtr & _dateTimeProvider, const Char * _format, Char * const _timestamp, size_t _capacity );
    }
}