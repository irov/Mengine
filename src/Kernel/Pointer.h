#pragma once

#include "Kernel/Mixin.h"

#ifdef MENGINE_DEBUG
#include "stdex/mpl.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class PointerT
    {
    public:
        PointerT( std::nullptr_t )
            : m_pointer( nullptr )
        {
        }

        PointerT( T * _pointer )
            : m_pointer( _pointer )
        {
        }

        PointerT( const PointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

    public:
        PointerT & operator = ( const PointerT & _pointer )
        {
            this->m_pointer = _pointer.m_pointer;

            return *this;
        }

    public:
        operator T * () const
        {
            return m_pointer;
        }

        template<class U>
        operator U * () const
        {
            U * t = static_cast<U *>(m_pointer);

            return t;
        }

    protected:
        T * m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ConstPointerT
    {
    public:
        ConstPointerT( std::nullptr_t )
            : m_pointer( nullptr )
        {
        }

        ConstPointerT( const T * _pointer )
            : m_pointer( _pointer )
        {
        }

        ConstPointerT( const ConstPointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

    public:
        ConstPointerT & operator = ( const ConstPointerT & _pointer )
        {
            this->m_pointer = _pointer.m_pointer;

            return *this;
        }

    public:
        operator const T * () const
        {
            return m_pointer;
        }

        template<class U>
        operator const U * () const
        {
            const U * t = static_cast<const U *>(m_pointer);

            return t;
        }

    protected:
        const T * m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class PointerT<IntrusivePtr<T> >
    {
    public:
        PointerT( std::nullptr_t )
        {
        }

        PointerT( T * _pointer )
            : m_pointer( _pointer )
        {
        }

        PointerT( const PointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

        PointerT( PointerT && _pointer )
            : m_pointer( std::forward<PointerT &&>( _pointer.m_pointer ) )
        {
        }

        PointerT( const IntrusivePtr<T> & _pointer )
            : m_pointer( _pointer )
        {
        }

        template<class U>
        PointerT( const IntrusivePtr<U> & _pointer )
            : m_pointer( _pointer )
        {
        }

        template<class U>
        PointerT( IntrusivePtr<U> && _pointer )
            : m_pointer( std::forward<IntrusivePtr<U> &&>( _pointer ) )
        {
        }

        template<class U, class D>
        PointerT( const IntrusivePtr<U, D> & _pointer )
            : m_pointer( _pointer )
        {
        }

        template<class U, class D>
        PointerT( IntrusivePtr<U, D> && _pointer )
            : m_pointer( std::forward<IntrusivePtr<U, D> &&>( _pointer ) )
        {
        }

    public:
        operator IntrusivePtr<T>() &&
        {
            return std::move( m_pointer );
        }

        template<class U>
        operator IntrusivePtr<U>() &&
        {            
#ifdef MENGINE_DEBUG
            T * p = m_pointer.get();

            if( p == nullptr )
            {
                return IntrusivePtr<U>();
            }

            if( stdex::mpl::is_dynamic_cast<U *>::test( p ) == false )
            {
                throw;
            }
#endif

            return IntrusivePtr<U>( std::move( m_pointer ) );
        }

        template<class D>
        operator IntrusivePtr<T, D>() &&
        {
            return IntrusivePtr<T, D>( std::move( m_pointer ) );
        }

        template<class U, class D>
        operator IntrusivePtr<U, D>() &&
        {
#ifdef MENGINE_DEBUG
            T * p = m_pointer.get();

            if( p == nullptr )
            {
                return IntrusivePtr<U, D>();
            }

            if( stdex::mpl::is_dynamic_cast<U *>::test( p ) == false )
            {
                throw;
            }
#endif

            return IntrusivePtr<U, D>( std::move( m_pointer ) );
        }

    protected:
        IntrusivePtr<T> m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef ConstPointerT<void> ConstPointer;
    typedef PointerT<void> Pointer;
    //////////////////////////////////////////////////////////////////////////
}
