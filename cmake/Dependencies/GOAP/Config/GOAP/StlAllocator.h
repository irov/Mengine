#pragma once

#include "GOAP/Allocator.h"

#include <memory>
#include <new>

namespace GOAP
{
    template <typename T>
    class StlAllocator
        : public std::allocator<T>
    {
    public:
        typedef size_t size_type;
        typedef T * pointer;
        typedef const T * const_pointer;

        template<typename U>
        struct rebind
        {
            typedef StlAllocator<U> other;
        };

    public:
        StlAllocator( Allocator * _allocator ) noexcept
            : std::allocator<T>()
            , m_allocator( _allocator )
        {
        }

        StlAllocator( const StlAllocator & a ) noexcept
            : std::allocator<T>( a )
            , m_allocator( a.m_allocator )
        {
        }
        
        template <class U> 
        StlAllocator( const StlAllocator<U> & a ) noexcept
            : std::allocator<T>( a )
            , m_allocator( a.m_allocator )
        {
        }

        ~StlAllocator() noexcept
        {
        }

    public:
        pointer allocate( std::size_t n, const void * = nullptr )
        {
            constexpr size_t element_size = sizeof( T );

            void * p = m_allocator->allocate( n * element_size );

            return static_cast<T *>(p);
        }

        void deallocate( pointer p, std::size_t )
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