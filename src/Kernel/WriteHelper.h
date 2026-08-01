#pragma once

#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        void writeUint16( void * _buffer, uint16_t _value );
        void writeUint32( void * _buffer, uint32_t _value );
        void writeUint64( void * _buffer, uint64_t _value );
    }
}
