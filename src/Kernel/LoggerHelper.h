#pragma once

#include "Kernel/DateTime.h"
#include "Kernel/LoggerLevel.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        Char getLoggerLevelSymbol( ELoggerLevel _level );
        size_t makeLoggerFullDate( const PlatformDateTime & _dateTime, const Char * _format, Char * const _timestamp, size_t _offset, size_t _capacity );
        size_t makeLoggerShortDate( const PlatformDateTime & _dateTime, const Char * _format, Char * const _timestamp, size_t _offset, size_t _capacity );
        size_t makeLoggerFunctionStamp( const Char * _file, uint32_t _line, const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity );
        size_t makeLoggerThreadStamp( const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity );
    }
}