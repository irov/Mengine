#pragma once

#include "Kernel/DateTime.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        size_t makeLoggerTimestamp( const PlatformDateTime & _dateTime, const Char * _format, Char * const _timestamp, size_t _capacity );
        size_t makeLoggerFunctionStamp( const Char * _file, uint32_t _line, Char * const _buffer, size_t _offset, size_t _capacity );
    }
}