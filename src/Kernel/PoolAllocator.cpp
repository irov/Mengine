#include "PoolAllocator.h"

#include "Kernel/AllocatorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void * PoolAllocator::malloc( size_t _size, const Char * _doc )
    {
        void * p = Helper::allocateMemory( _size, _doc );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * PoolAllocator::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        void * p = Helper::reallocateMemory( _mem, _size, _doc );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void PoolAllocator::free( void * _ptr, const Char * _doc )
    {
        Helper::deallocateMemory( _ptr, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
}