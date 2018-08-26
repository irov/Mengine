#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Mengine
{
    namespace Helper
    {
        uint32_t make_crc32( const void * _buffer, size_t _size );
    }
}