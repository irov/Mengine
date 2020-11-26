#pragma once

#include "GOAP/Allocator.h"

#include <new>

namespace GOAP
{
    template <typename T>
    class StlAllocator
    {
    public:
        typedef T value_type;

        StlAllocator( Allocator * _allocator )
            : m_allocator( _allocator )
        {
        }

        template <class U> StlAllocator( const StlAllocator<U> & a ) noexcept
            : m_allocator( a.m_allocator )
        {
        }

    public:
        T * allocate( std::size_t n, const void * = nullptr )
        {
            void * p = m_allocator->allocate( n );

            return static_cast<T *>(p);
        }

        void deallocate( T * p, std::size_t )
        {
            m_allocator->deallocate( p );
        }

    public:
        Allocator * m_allocator;
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