#pragma once

#include "Core/IntrusivePtr.h"

#include "stdex/mpl.h"

namespace Mengine
{
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
#ifndef NDEBUG
            if( stdex::mpl::is_dynamic_cast<U *>::test( m_pointer ) == false )
            {
                throw;
            }
#endif

            U * t = static_cast<U *>(m_pointer);

            return t;
        }

        template<class U>
        operator IntrusivePtr<U>() const
        {
#ifndef NDEBUG
            if( stdex::mpl::is_dynamic_cast<U *>::test( m_pointer ) == false )
            {
                throw;
            }
#endif

            U * t = static_cast<U *>(m_pointer);

            return IntrusivePtr<U>( t );
        }


    protected:
        T * m_pointer;
    };

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
            if( stdex::mpl::is_dynamic_cast<U *>::test( p ) == false )
            {
                throw;
            }
#endif

            U * t = static_cast<U *>(p);

            return IntrusivePtr<U>( t );
        }

    protected:
        IntrusivePtr<T> m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef PointerT<void> Pointer;
}
