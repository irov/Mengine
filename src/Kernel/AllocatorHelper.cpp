#include "AllocatorHelper.h"

#include "Interface/AllocatorServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void * allocateMemory( size_t _size, const Char * _doc )
        {
            void * memory_buffer = ALLOCATOR_SERVICE()
                ->malloc( _size, _doc );

            return memory_buffer;
        }
        //////////////////////////////////////////////////////////////////////////
        void * reallocateMemory( void * _buffer, size_t _size, const Char * _doc )
        {
            void * memory_buffer = ALLOCATOR_SERVICE()
                ->realloc( _buffer, _size, _doc );

            return memory_buffer;
        }
        //////////////////////////////////////////////////////////////////////////
        void freeMemory( void * _memory, const Char * _doc )
        {
            ALLOCATOR_SERVICE()
                ->free( _memory, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}