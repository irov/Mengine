#pragma once

#include "Kernel/Typename.h"
#include "Kernel/AllocatorHelper.h"

#include "Config/Typeinfo.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * allocateT()
        {
#ifdef MENGINE_WINDOWS_DEBUG
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            size_t memory_size = sizeof( T );
            void * memory_buffer = Helper::allocateMemory( memory_size, doc );
            
            return reinterpret_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void freeT( T * _ptr )
        {
#ifdef MENGINE_WINDOWS_DEBUG
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            Helper::deallocateMemory( _ptr, doc );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * allocateArrayT( size_t _count )
        {
#ifdef MENGINE_WINDOWS_DEBUG
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            size_t memory_size = sizeof( T ) * _count;
            void * memory_buffer = Helper::allocateMemory( memory_size, doc );

            return reinterpret_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * callocateArrayT( size_t _count )
        {
#ifdef MENGINE_WINDOWS_DEBUG
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            void * memory_buffer = Helper::callocateMemory( _count, sizeof( T ), doc );

            return reinterpret_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * reallocateArrayT( T * _array, size_t _count )
        {
#ifdef MENGINE_WINDOWS_DEBUG
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            size_t memory_size = sizeof( T ) * _count;
            void * memory_buffer = Helper::reallocateMemory( _array, memory_size, doc );

            return reinterpret_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void freeArrayT( T * _array )
        {
#ifdef MENGINE_WINDOWS_DEBUG
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            Helper::deallocateMemory( _array, doc );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class ... Args>
        T * newT( Args && ... _args )
        {
            void * memory_buffer = Helper::allocateT<T>();

            new (memory_buffer)T( std::forward<Args &&>( _args ) ... );

            return std::launder( static_cast<T *>(memory_buffer) );
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