#pragma once

#include "Kernel/Typename.h"
#include "Kernel/AllocatorHelper.h"

#include "Config/Config.h"
#include "Config/Char.h"

#include <utility>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * allocateT()
        {
            size_t memory_size = sizeof( T );
            void * memory_buffer = Helper::allocateMemory( memory_size, Typename<T>::value );
            
            return reinterpret_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void freeT( T * _ptr )
        {
            Helper::freeMemory( _ptr, Typename<T>::value );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * allocateArrayT( size_t _count )
        {
            size_t memory_size = sizeof( T ) * _count;
            void * memory_buffer = Helper::allocateMemory( memory_size, Typename<T>::value );

            return reinterpret_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * reallocateArrayT( T * _array, size_t _count )
        {
            size_t memory_size = sizeof( T ) * _count;
            void * memory_buffer = Helper::reallocateMemory( _array, memory_size, Typename<T>::value );

            return reinterpret_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void freeArrayT( T * _array )
        {
            Helper::freeMemory( _array, Typename<T>::value );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class ... Args>
        T * newT( Args && ... _args )
        {
            void * memory_buffer = Helper::allocateT<T>();

            new (memory_buffer)T( std::forward<Args &&>( _args ) ... );

            return reinterpret_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void deleteT( T * _t )
        {
            _t->~T();

            Helper::freeT( _t );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}