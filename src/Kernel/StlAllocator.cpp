#include "StlAllocator.h"

#include "Interface/AllocatorServiceInterface.h"

namespace Mengine
{
    namespace Detail
    {
        void * stl_allocate( std::size_t _size )
        {
            void * p = ALLOCATOR_SERVICE()
                ->malloc( _size, "stl" );

            return p;
        }

        void stl_deallocate( void * _ptr )
        {
            ALLOCATOR_SERVICE()
                ->free( _ptr, "stl" );
        }
    }
}