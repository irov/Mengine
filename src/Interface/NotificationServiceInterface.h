#pragma once

#include "Config/Typedef.h"
#include "Config/Tuple.h"

#include "Interface/ServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factorable.h"
#include "Kernel/Observable.h"
#include "Kernel/Exception.h"
#include "Kernel/IntrusivePtr.h"

#include <type_traits>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ExceptionNotificationFailed
    {
    };
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
        : public ArgsObserverCallable<Args ...>
    {
    public:
        MethodObserverCallable( C * _self, M _method )
            : m_self( _self )
            , m_method( _method )
        {
            IntrusivePtrBase::intrusive_ptr_add_ref( m_self );
        }

        ~MethodObserverCallable()
        {
            IntrusivePtrBase::intrusive_ptr_dec_ref( m_self );
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
    template<class C, class ... P>
    class GeneratorMethodObserverCallable<void (C::*)(P...)>
        : public MethodObserverCallable < C, void (C::*)(P...), std::remove_const_t<std::remove_reference_t<P>> ... >
    {
    public:
        GeneratorMethodObserverCallable( C * _self, void (C::*_method)(P...) )
            : MethodObserverCallable<C, void (C::*)(P...), std::remove_const_t<std::remove_reference_t<P>> ...>( _self, _method )
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class ... P>
    class GeneratorMethodObserverCallable<void (C::*)(P...) const>
        : public MethodObserverCallable < C, void (C::*)(P...) const, std::remove_const_t<std::remove_reference_t<P>> ... >
    {
    public:
        GeneratorMethodObserverCallable( C * _self, void (C::*_method)(P...) const )
            : MethodObserverCallable<C, void (C::*)(P...) const, std::remove_const_t<std::remove_reference_t<P>> ...>( _self, _method )
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
        typedef ArgsObserverCallable< std::remove_const_t<std::remove_reference_t<Args>> ...> args_callable_type;

    public:
        ArgsObserverVisitorCallable( const Tuple<Args...> & _args )
            : m_args( _args )
        {
        }

    protected:
        void visit( const ObserverCallableInterfacePtr & _callable ) override
        {
            args_callable_type * callable_args = _callable.getT<args_callable_type *>();

            this->call_with_tuple( callable_args, std::make_integer_sequence<uint32_t, sizeof ... (Args)>() );
        }

        template<uint32_t ... I>
        void call_with_tuple( args_callable_type * _callable, std::integer_sequence<uint32_t, I...> )
        {
            _callable->call( std::get<I>( m_args )... );
        }

    protected:
        const Tuple<Args...> & m_args;
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
            ObserverCallableInterfacePtr callable( new GeneratorMethodObserverCallable<M>( _self, _method ) );

            this->addObserver( _id, ObservablePtr( _self ), callable );
        }

    public:
        virtual bool visitObservers( uint32_t _id, ObserverVisitorCallableInterface * _visitor ) = 0;

    public:
        template<class ... Args>
        inline bool notify( uint32_t _id, const Args & ... _args )
        {
            bool successful = this->notify_tuple( _id, std::make_tuple( _args... ) );

            return successful;
        }

        template<class ... Args>
        inline bool notify_tuple( uint32_t _id, const Tuple<Args...> & _args )
        {
            ArgsObserverVisitorCallable<Args...> visitor( _args );

            bool successful = this->visitObservers( _id, &visitor );

            return successful;
        }
    };
}
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_SERVICE()\
    ((Mengine::NotificationServiceInterface*)SERVICE_GET(Mengine::NotificationServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_ADDOBSERVERMETHOD( ID, Observer, Method )\
    NOTIFICATION_SERVICE()->addObserverMethod( ID, Observer, Method )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_REMOVEOBSERVER( ID, Observer )\
    NOTIFICATION_SERVICE()->removeObserver( ID, Observer )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_REMOVEOBSERVER_THIS( ID )\
    NOTIFICATION_SERVICE()->removeObserver( ID, Mengine::ObservablePtr(this) )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_NOTIFY( ID, ARGS )\
    NOTIFICATION_SERVICE()->notify_tuple( ID, std::make_tuple ARGS )
//////////////////////////////////////////////////////////////////////////
