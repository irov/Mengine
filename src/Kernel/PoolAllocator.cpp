#include "PoolAllocator.h"

#include "Interface/AllocatorServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void * PoolAllocator::malloc( size_t _size, const char * _doc )
    {
        void * p = ALLOCATOR_SERVICE()
            ->malloc( _size, _doc );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * PoolAllocator::realloc( void * _mem, size_t _size, const char * _doc )
    {
        void * p = ALLOCATOR_SERVICE()
            ->realloc( _mem, _size, _doc );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void PoolAllocator::free( void * _ptr, const char * _doc )
    {
        ALLOCATOR_SERVICE()
            ->free( _ptr, _doc );
    }
}