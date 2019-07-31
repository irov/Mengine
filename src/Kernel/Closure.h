#pragma once

#include "Kernel/IntrusivePtr.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class P, class M>
    class Closure
    {
    public:
        Closure( P * _ptr, M _method )
            : m_ptr( _ptr )
            , m_method( _method )
        {
        }

    public:
        template<class ... Args>
        auto operator () ( Args && ... _args ) const
        {
            return (m_ptr->*m_method)(std::forward<Args &&>( _args )...);
        }


    protected:
        P * m_ptr;
        M m_method;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class P, class D, class M>
    class ClosurePtr
    {
    public:
        ClosurePtr( const IntrusivePtr<P, D> & _ptr, M _method )
            : m_ptr( _ptr )
            , m_method( _method )
        {
        }

    public:
        template<class ... Args>
        auto operator () ( Args && ... _args ) const
        {
            P * ptr = m_ptr.get();

            return (ptr->*m_method)(std::forward<Args &&>( _args )...);
        }

    protected:
        IntrusivePtr<P, D> m_ptr;
        M m_method;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class P, class M>
        Closure<P, M> closure( P * _ptr, M _method )
        {
            return Closure<P, M>( _ptr, _method );
        }

        template<class P, class D, class M>
        ClosurePtr<P, D, M> closure( const IntrusivePtr<P, D> & _ptr, M _method )
        {
            return ClosurePtr<P, D, M>( _ptr, _method );
        }
    }
}