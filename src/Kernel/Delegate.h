#pragma once

#include "Kernel/IntrusivePtr.h"

#include "Config/Lambda.h"
#include "Config/Tuple.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class P, class M, class Forwards>
    class Delegate
    {
    public:
        Delegate( P * _ptr, M _method, Forwards && _forwards )
            : m_ptr( _ptr )
            , m_method( _method )
            , m_forwards( std::forward<Forwards &&>( _forwards ) )
        {
        }

    public:
        template<class ... Args>
        auto operator () ( Args && ... _args ) const
        {
            return std::apply( m_method, std::tuple_cat( std::make_tuple( m_ptr ), std::tuple_cat( std::make_tuple( std::forward<Args &&>( _args ) ... ), m_forwards ) ) );
        }

    protected:
        P * m_ptr;
        M m_method;
        Forwards m_forwards;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class P, class D, class M, class Forwards>
    class DelegatePtr
    {
    public:
        DelegatePtr( const IntrusivePtr<P, D> & _ptr, M _method, Forwards && _forwards )
            : m_ptr( _ptr )
            , m_method( _method )
            , m_forwards( std::forward<Forwards &&>( _forwards ) )
        {
        }

    public:
        template<class ... Args>
        auto operator () ( Args && ... _args ) const
        {
            P * ptr = m_ptr.get();

            return std::apply( m_method, std::tuple_cat( std::make_tuple( ptr ), std::tuple_cat( std::make_tuple( std::forward<Args &&>( _args ) ... ), m_forwards ) ) );
        }

    protected:
        IntrusivePtr<P, D> m_ptr;
        M m_method;
        Forwards m_forwards;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class P, class M, class ... Forwards>
        Delegate<P, M, Tuple<Forwards ...>> delegate( P * _ptr, M _method, Forwards ... _args )
        {
            return Delegate<P, M, Tuple<Forwards ...>>( _ptr, _method, std::make_tuple( std::forward<Forwards &&>( _args ) ... ) );
        }

        template<class P, class D, class M, class ... Forwards>
        DelegatePtr<P, D, M, Tuple<Forwards ...>> delegate( const IntrusivePtr<P, D> & _ptr, M _method, Forwards ... _args )
        {
            return DelegatePtr<P, D, M, Tuple<Forwards ...>>( _ptr, _method, std::make_tuple( std::forward<Forwards &&>( _args ) ... ) );
        }
    }
}