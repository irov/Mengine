#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "Factory/FactorableUnique.h"
#include "Factory/Factorable.h"

#include "Kernel/Observable.h"

#include "Core/IntrusivePtr.h"

#include <tuple>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ObserverCallableInterface
        : public FactorableUnique<Factorable>
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ObserverCallableInterface> ObserverCallableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    template<class ... Args>
    class ArgsObserverCallable
        : public ObserverCallableInterface
    {
    public:
        virtual void call( Args ... _args ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class M, class ... Args>
    class MethodObserverCallable
        : public ArgsObserverCallable < Args... >
    {
    public:
        MethodObserverCallable( C * _self, M _method )
            : m_self( _self )
            , m_method( _method )
        {
            stdex::intrusive_this_acquire( m_self );
        }

        ~MethodObserverCallable()           
        {
            stdex::intrusive_this_release( m_self );
        }

    protected:
        void call( Args ... _args ) override
        {
            (m_self->*m_method)(_args...);
        }

    protected:
        C * m_self;
        M m_method;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class GeneratorMethodObserverCallable;
    //////////////////////////////////////////////////////////////////////////
    template<class C>
    class GeneratorMethodObserverCallable<void (C::*)()>
        : public MethodObserverCallable< C, void (C::*)() >
    {
    public:
        GeneratorMethodObserverCallable( C * _self, void (C::*_method)() )
            : MethodObserverCallable<C, void (C::*)()>( _self, _method )
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P0>
    class GeneratorMethodObserverCallable<void (C::*)(P0)>
        : public MethodObserverCallable < C, void (C::*)(P0), typename std::remove_reference<P0>::type >
    {
    public:
        GeneratorMethodObserverCallable( C * _self, void (C::*_method)(P0) )
            : MethodObserverCallable<C, void (C::*)(P0), typename std::remove_reference<P0>::type>( _self, _method )
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P0, class P1>
    class GeneratorMethodObserverCallable<void (C::*)(P0, P1)>
        : public MethodObserverCallable< C, void (C::*)(P0, P1), typename std::remove_reference<P0>::type, typename std::remove_reference<P1>::type >
    {
    public:
        GeneratorMethodObserverCallable( C * _self, void (C::*_method)(P0, P1) )
            : MethodObserverCallable<C, void (C::*)(P0, P1), typename std::remove_reference<P0>::type, typename std::remove_reference<P1>::type>( _self, _method )
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    class ObserverVisitorCallableInterface
    {
    public:
        virtual void visit( const ObserverCallableInterfacePtr & _callable ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ObserverVisitorCallableInterface> ObserverVisitorCallableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    template<class ... Args>
    class ArgsObserverVisitorCallable
        : public ObserverVisitorCallableInterface
    {
        typedef ArgsObserverCallable<Args...> args_callable_type;

    public:
        ArgsObserverVisitorCallable( Args ... _args )
            : m_args( _args... )
        {
        }

    protected:
        void visit( const ObserverCallableInterfacePtr & _callable ) override
        {
            args_callable_type * callable_args = _callable.getT<args_callable_type *>();

            this->call_with_tuple( callable_args, std::index_sequence_for<Args...>() );
        }

        template<std::size_t... Is>
        void call_with_tuple( args_callable_type * _callable, std::index_sequence<Is...> )
        {
            _callable->call( std::get<Is>( m_args )... );
        }

    protected:
        std::tuple<Args...> m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    class NotificationServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "NotificationService" )

    public:
        virtual void addObserver( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable ) = 0;
        virtual void removeObserver( uint32_t _id, const ObservablePtr & _observer ) = 0;

    public:
        template<class C, class M>
        void addObserverMethod( uint32_t _id, C * _self, M _method )
        {
            ObserverCallableInterfacePtr callable =
                new GeneratorMethodObserverCallable<M>( _self, _method );

            this->addObserver( _id, _self, callable );
        }

    public:
        virtual void visitObservers( uint32_t _id, ObserverVisitorCallableInterface * _visitor ) = 0;

    public:
        template<class ... Args>
        inline void notify( uint32_t _id, const Args & ... _args )
        {
            ArgsObserverVisitorCallable<Args...> visitor{ _args... };

            this->visitObservers( _id, &visitor );
        }
    };
    //////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_SERVICE()\
    ((Mengine::NotificationServiceInterface*)SERVICE_GET(Mengine::NotificationServiceInterface))
}