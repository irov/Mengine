#pragma once

#include "Config/StdDef.h"
#include "Config/Char.h"

namespace Mengine
{
    class PoolAllocator
    {
    public:
        static void * malloc( size_t _size, const Char * _doc );
        static void * realloc( void * _mem, size_t _size, const Char * _doc );
        static void free( void * _ptr, const Char * _doc );
    };
}