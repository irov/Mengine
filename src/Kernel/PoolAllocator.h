#pragma once

namespace Mengine
{
    class PoolAllocator
    {
    public:
        static void * malloc( size_t _size, const char * _doc );
        static void * realloc( void * _mem, size_t _size, const char * _doc );
        static void free( void * _ptr, const char * _doc );
    };
}