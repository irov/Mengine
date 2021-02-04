#pragma once

#ifndef METABUF_MEMCPY
#   include <algorithm>
#   define METABUF_MEMCPY(Dest, Src, Size) std::copy((const uint8_t *)Src, (const uint8_t *)Src + Size, (uint8_t *)Dest)
#endif

#include <vector>
#include <exception>

extern void * _metabuf_malloc( size_t _size );
extern void _metabuf_free( void * _ptr );

#ifndef METABUF_MALLOC
#define METABUF_MALLOC(S) (_metabuf_malloc(S))
#endif

#ifndef METABUF_FREE
#define METABUF_FREE(B, S) (_metabuf_free(B))
#endif

namespace Metabuf
{
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
            void * p = _metabuf_malloc( total_size );
            return static_cast<T *>(p);
        }

        void deallocate( T * p, std::size_t )
        {
            _metabuf_free( p );
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

    template<class Type, class Allocator = StlAllocator<Type>>
    using Vector = std::vector<Type, Allocator>;

    using Exception = std::exception;
}