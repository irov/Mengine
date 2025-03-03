#pragma once

#include "Kernel/AllocatorHelper.h"

#include "Config/New.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<typename T>
    class StlAllocator
    {
    public:
        typedef T value_type;

        StlAllocator() = default;
        template <class U> StlAllocator( const StlAllocator<U> & ) noexcept
        {
        }

        T * allocate( std::size_t n, const void * = nullptr )
        {
            std::size_t element_size = sizeof( T );
            std::size_t total_size = element_size * n;
            void * p = Helper::allocateMemory( total_size, "stl" );
            return static_cast<T *>(p);
        }

        void deallocate( T * p, std::size_t )
        {
            Helper::deallocateMemory( p, "stl" );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    bool operator == ( const StlAllocator<T> &, const StlAllocator<T> & )
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    bool operator != ( const StlAllocator<T> &, const StlAllocator<T> & )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}