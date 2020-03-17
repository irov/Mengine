#pragma once

#include <new>
#include <limits>
#include <memory>

namespace Mengine
{
    namespace Detail
    {
        void * stl_allocate( std::size_t _size );
        void stl_deallocate( void * _ptr );
    }

    template <typename T>
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
            void * p = Detail::stl_allocate( total_size );
            return static_cast<T *>(p);
        }

        void deallocate( T * p, std::size_t )
        {
            Detail::stl_deallocate( p );
        }
    };

    template <class T>
    bool operator == ( const StlAllocator<T> &, const StlAllocator<T> & )
    {
        return true;
    }

    template <class T>
    bool operator != ( const StlAllocator<T> &, const StlAllocator<T> & )
    {
        return false;
    }
}