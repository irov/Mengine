#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"
#include "Config/StdDef.h"
#include "Config/Utility.h"

#include <new>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void * allocateMemory( size_t _size, const Char * _doc );
        void * callocateMemory( size_t _count, size_t _size, const Char * _doc );
        void * reallocateMemory( void * _buffer, size_t _size, const Char * _doc );
        void deallocateMemory( void * _memory, const Char * _doc );
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * allocateMemoryT( const Char * _doc )
        {
            const size_t element_size = sizeof( T );
            void * ptr = Helper::allocateMemory( element_size, _doc );

            return static_cast<T *>(ptr);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * allocateMemoryNT( uint32_t _count, const Char * _doc )
        {
            const size_t element_size = sizeof( T );
            const size_t memory_size = element_size * _count;
            void * ptr = Helper::allocateMemory( memory_size, _doc );

            return static_cast<T *>(ptr);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * callocateMemoryNT( uint32_t _count, const Char * _doc )
        {
            const size_t element_size = sizeof( T );
            void * ptr = Helper::callocateMemory( _count, element_size, _doc );

            return static_cast<T *>(ptr);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class ... Args>
        T * newMemoryT( const Char * _doc, Args && ... _args )
        {
            size_t element_size = sizeof( T );
            void * memory_buffer = Helper::allocateMemory( element_size, _doc );

            new (memory_buffer)T( std::forward<Args>( _args ) ... );

            return static_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void deleteMemoryT( T * _ptr, const Char * _doc )
        {
            _ptr->~T();

            Helper::deallocateMemory( _ptr, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}