#include "PoolAllocator.h"

#include "Kernel/AllocatorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void * PoolAllocator::malloc( size_t _size, const char * _doc )
    {
        void * p = Helper::allocateMemory( _size, _doc );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * PoolAllocator::realloc( void * const _mem, size_t _size, const char * _doc )
    {
        void * p = Helper::reallocateMemory( _mem, _size, _doc );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void PoolAllocator::free( void * const _ptr, const char * _doc )
    {
        Helper::deallocateMemory( _ptr, _doc );
    }
}