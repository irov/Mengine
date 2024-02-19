#pragma once

#include "Kernel/LoggerLevel.h"
#include "Kernel/ConstString.h"

#include "Config/Char.h"
#include "Config/StdInt.h"
#include "Config/Timestamp.h"

namespace Mengine
{
    namespace Helper
    {
        Char getLoggerLevelSymbol( ELoggerLevel _level );
        size_t makeLoggerFullDate( Timestamp _timestamp, const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity );
        size_t makeLoggerShortDate( Timestamp _timestamp, const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity );
        size_t makeLoggerFunctionStamp( const Char * _function, uint32_t _line, const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity );
        size_t makeLoggerThreadStamp( const ConstString & _threadName, const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity );
    }
}