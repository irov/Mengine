#pragma once

#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        void readUint32( const void * _buffer, uint32_t * const _value );
        void readUint64( const void * _buffer, uint64_t * const _value );
    }
}