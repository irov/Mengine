#pragma once

#include "Interface/DateTimeProviderInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t makeLoggerDateTimestamp( const DateTimeProviderInterfacePtr & _dateTimeProvider, Char * const _timestamp, size_t _capacity );
        size_t makeFilePathDateTimestamp( const DateTimeProviderInterfacePtr & _dateTimeProvider, Char * const _timestamp, size_t _capacity );
    }
}