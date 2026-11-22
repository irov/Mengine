#pragma once

#include "Kernel/UUIDv4.h"

namespace Mengine
{
    namespace Helper
    {
        bool parseUUIDv4( const Char * _value, size_t _size, UUIDv4 * const _uuid );
    }
}
