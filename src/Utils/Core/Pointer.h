#pragma once

#include "stdex/mpl.h"
#include "stdex/intrusive_ptr.h"

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
        operator stdex::intrusive_ptr<U>() const
        {
#ifndef NDEBUG
            if( stdex::mpl::is_dynamic_cast<U *>::test( m_pointer ) == false )
            {
                throw;
            }
#endif

            U * t = static_cast<U *>(m_pointer);

            return stdex::intrusive_ptr<U>( t );
        }


    protected:
        T * m_pointer;
    };

    template<class T>
    class PointerT<stdex::intrusive_ptr<T> >
    {
    public:
        PointerT( const stdex::intrusive_ptr<T> & _pointer )
            : m_pointer( _pointer )
        {
        }

        PointerT( const PointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

    public:
        operator stdex::intrusive_ptr<T>() const
        {
            return m_pointer;
        }

        template<class U>
        operator stdex::intrusive_ptr<U>() const
        {
            T * p = m_pointer.get();

#ifndef NDEBUG
            if( stdex::mpl::is_dynamic_cast<U *>::test( p ) == false )
            {
                throw;
            }
#endif

            U * t = static_cast<U *>(p);

            return stdex::intrusive_ptr<U>( t );
        }

    protected:
        stdex::intrusive_ptr<T> m_pointer;
    };

    typedef PointerT<void> Pointer;
}
