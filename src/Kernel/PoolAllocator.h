#pragma once

#include "Config/StdDef.h"

namespace Mengine
{
    class PoolAllocator
    {
    public:
        static void * malloc( size_t _size, const char * _doc );
        static void * realloc( void * const _mem, size_t _size, const char * _doc );
        static void free( void * const _ptr, const char * _doc );
    };
}