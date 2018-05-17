#pragma once

#include "Core/IntrusivePtr.h"

#ifndef NDEBUG
#   include "stdex/mpl.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class PointerT
    {
    public:
        PointerT( T * _pointer )
            : m_pointer( _pointer )
        {
        }

        PointerT( const PointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
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
    class PointerT<IntrusivePtr<T> >
    {
    public:
        PointerT( const IntrusivePtr<T> & _pointer )
            : m_pointer( _pointer )
        {
        }

        PointerT( const PointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

    public:
        operator IntrusivePtr<T>() const
        {
            return m_pointer;
        }

        template<class U>
        operator IntrusivePtr<U>() const
        {
            T * p = m_pointer.get();

#ifndef NDEBUG
            if( p == nullptr )
            {
                return IntrusivePtr<U>();
            }

            if( stdex::mpl::is_dynamic_cast<U *>::test( p ) == false )
            {
                throw;
            }
#endif

            U * t = static_cast<U *>(p);

            return IntrusivePtr<U>( t );
        }

        template<class B>
        operator IntrusiveDerivedPtr<T, B>() const
        {
            return IntrusiveDerivedPtr<T, B>( m_pointer );
        }

        template<class U, class B>
        operator IntrusiveDerivedPtr<U, B>() const
        {
            T * p = m_pointer.get();

#ifndef NDEBUG
            if( p == nullptr )
            {
                return IntrusivePtr<U>();
            }

            if( stdex::mpl::is_dynamic_cast<U *>::test( p ) == false )
            {
                throw;
            }
#endif

            U * u = static_cast<U *>(p);

            return IntrusiveDerivedPtr<U, B>( u );
        }

    protected:
        IntrusivePtr<T> m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef PointerT<void> Pointer;
    //////////////////////////////////////////////////////////////////////////
}
