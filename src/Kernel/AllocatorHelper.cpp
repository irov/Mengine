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
        void * callocateMemory( size_t _num, size_t _size, const Char * _doc )
        {
            void * memory_buffer = ALLOCATOR_SERVICE()
                ->calloc( _num, _size, _doc );

            return memory_buffer;
        }
        //////////////////////////////////////////////////////////////////////////
        void * reallocateMemory( void * const _buffer, size_t _size, const Char * _doc )
        {
            void * memory_buffer = ALLOCATOR_SERVICE()
                ->realloc( _buffer, _size, _doc );

            return memory_buffer;
        }
        //////////////////////////////////////////////////////////////////////////
        void deallocateMemory( void * const _memory, const Char * _doc )
        {
            ALLOCATOR_SERVICE()
                ->free( _memory, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}