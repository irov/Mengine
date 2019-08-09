#pragma once

#include "Config/Char.h"
#include "Kernel/Typename.h"

#include "stdex/allocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class MemoryAllocator
    {
    public:
        void * operator new (size_t _size)
        {
            return stdex_malloc( _size, Typename<T>::value );
        }

        void operator delete (void * _ptr, size_t _size)
        {
            MENGINE_UNUSED( _size );

            stdex_free( _ptr, Typename<T>::value );
        }

        void * operator new []( size_t _size )
        {
            return stdex_malloc( _size, Typename<T>::value );
        }

            void operator delete []( void * _ptr, size_t _size )
        {
            MENGINE_UNUSED( _size );

            stdex_free( _ptr, Typename<T>::value );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T, class ... Args>
        T * allocateT( Args && ... _args )
        {
            size_t memory_size = sizeof( T );
            void * memory_buffer = stdex_malloc( memory_size, Typename<T>::value );

            new (memory_buffer)T( std::forward<Args>( _args ) ... );

            return reinterpret_cast<T *>(memory_buffer);
        }

        template<class T>
        void freeT( T * _t )
        {
            _t->~T();

            stdex_free( _t, Typename<T>::value );
        }

        template<class T>
        T * allocateArrayT( size_t _count )
        {
            size_t memory_size = sizeof( T ) * _count;
            void * memory_buffer = stdex_malloc( memory_size, Typename<T>::value );

            return reinterpret_cast<T *>(memory_buffer);
        }

        template<class T>
        T * reallocateArrayT( T * _buffer, size_t _count )
        {
            size_t memory_size = sizeof( T ) * _count;
            void * memory_buffer = stdex_realloc( _buffer, memory_size, Typename<T>::value );

            return reinterpret_cast<T *>(memory_buffer);
        }

        template<class T>
        void freeArrayT( T * _memory )
        {
            stdex_free( _memory, Typename<T>::value );
        }

        MENGINE_INLINE void * allocateMemory( size_t _size, const Char * _doc )
        {
            void * memory_buffer = stdex_malloc( _size, _doc );

            return memory_buffer;
        }

        MENGINE_INLINE void * reallocateMemory( void * _buffer, size_t _size, const Char * _doc )
        {
            MENGINE_UNUSED( _doc );
            void * memory_buffer = stdex_realloc( _buffer, _size, _doc );

            return memory_buffer;
        }

        MENGINE_INLINE void freeMemory( void * _memory, const Char * _doc )
        {
            stdex_free( _memory, _doc );
        }
    }
}