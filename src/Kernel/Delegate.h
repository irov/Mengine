#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/Tuple.h"

#include "Config/Lambda.h"

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
            , m_forwards( std::forward<Forwards>( _forwards ) )
        {
        }

    public:
        template<class ... Args>
        auto operator () ( Args && ... _args ) const
        {
            return std::apply( [this, &_args...]( const auto & ... _forwards )
            {
                return std::invoke( m_method, m_ptr,
                    std::forward<Args>( _args ) ..., _forwards ... );
            }, m_forwards );
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
            , m_forwards( std::forward<Forwards>( _forwards ) )
        {
        }

    public:
        template<class ... Args>
        auto operator () ( Args && ... _args ) const
        {
            return std::apply( [this, &_args...]( const auto & ... _forwards )
            {
                return std::invoke( m_method, m_ptr.get(),
                    std::forward<Args>( _args ) ..., _forwards ... );
            }, m_forwards );
        }

    protected:
        IntrusivePtr<P, D> m_ptr;
        M m_method;
        Forwards m_forwards;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class P, class M, class ... Forwards>
        DelegatePtr<std::remove_const_t<P>, void, M, Tuple<std::remove_reference_t<Forwards> ...>> delegate( P * _ptr, M _method, Forwards && ... _args )
        {
            return DelegatePtr<std::remove_const_t<P>, void, M, Tuple<std::remove_reference_t<Forwards> ...>>( IntrusivePtr<std::remove_const_t<P>>::from( _ptr ), _method, std::forward_as_tuple( std::forward<Forwards>( _args ) ... ) );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class P, class D, class M, class ... Forwards>
        DelegatePtr<P, D, M, Tuple<std::remove_reference_t<Forwards> ...>> delegate( const IntrusivePtr<P, D> & _ptr, M _method, Forwards && ... _args )
        {
            return DelegatePtr<P, D, M, Tuple<std::remove_reference_t<Forwards> ...>>( _ptr, _method, std::forward_as_tuple( std::forward<Forwards>( _args ) ... ) );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class P, class M, class ... Forwards>
        Delegate<P, M, Tuple<std::remove_reference_t<Forwards> ...>> delegate_ptr( P * _ptr, M _method, Forwards && ... _args )
        {
            return Delegate<P, M, Tuple<std::remove_reference_t<Forwards> ...>>( _ptr, _method, std::forward_as_tuple( std::forward<Forwards>( _args ) ... ) );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}