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
#if defined(MENGINE_WINDOWS_DEBUG)
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            size_t element_size = sizeof( T );
            void * memory_buffer = Helper::allocateMemory( element_size, doc );

            return static_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void freeT( T * _ptr )
        {
#if defined(MENGINE_WINDOWS_DEBUG)
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
#if defined(MENGINE_WINDOWS_DEBUG)
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            const size_t element_size = sizeof( T );
            const size_t memory_size = element_size * _count;
            void * memory_buffer = Helper::allocateMemory( memory_size, doc );

            return static_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * callocateArrayT( size_t _count )
        {
#if defined(MENGINE_WINDOWS_DEBUG)
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            const size_t element_size = sizeof( T );
            void * memory_buffer = Helper::callocateMemory( _count, element_size, doc );

            return static_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * reallocateArrayT( T * _ptr, size_t _count )
        {
#if defined(MENGINE_WINDOWS_DEBUG)
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            const size_t element_size = sizeof( T );
            const size_t memory_size = element_size * _count;
            void * memory_buffer = Helper::reallocateMemory( _ptr, memory_size, doc );

            return static_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void freeArrayT( T * _ptr )
        {
#if defined(MENGINE_WINDOWS_DEBUG)
            const Char * doc = MENGINE_TYPEINFO_NAME( T );
#else
            const Char * doc = nullptr;
#endif

            Helper::deallocateMemory( _ptr, doc );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class ... Args>
        T * newT( Args && ... _args )
        {
            void * memory_buffer = Helper::allocateT<T>();

            new (memory_buffer)T( std::forward<Args>( _args ) ... );

            return static_cast<T *>(memory_buffer);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void deleteT( T * _ptr )
        {
            _ptr->~T();

            Helper::freeT( _ptr );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}