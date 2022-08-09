#include "AllocatorHelper.h"

#include "Interface/AllocatorSystemInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void * allocateMemory( size_t _size, const Char * _doc )
        {
            void * memory_buffer = ALLOCATOR_SYSTEM()
                ->malloc( _size, _doc );

            return memory_buffer;
        }
        //////////////////////////////////////////////////////////////////////////
        void * callocateMemory( size_t _count, size_t _size, const Char * _doc )
        {
            void * memory_buffer = ALLOCATOR_SYSTEM()
                ->calloc( _count, _size, _doc );

            return memory_buffer;
        }
        //////////////////////////////////////////////////////////////////////////
        void * reallocateMemory( void * _buffer, size_t _size, const Char * _doc )
        {
            void * memory_buffer = ALLOCATOR_SYSTEM()
                ->realloc( _buffer, _size, _doc );

            return memory_buffer;
        }
        //////////////////////////////////////////////////////////////////////////
        void deallocateMemory( void * _memory, const Char * _doc )
        {
            ALLOCATOR_SYSTEM()
                ->free( _memory, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}