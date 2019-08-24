#pragma once

#include "Config/Typedef.h"
#include "Config/Tuple.h"
#include "Config/Lambda.h"

#include "Interface/ServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factorable.h"
#include "Kernel/Observable.h"
#include "Kernel/Exception.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/Document.h"

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
    template<uint32_t ID>
    class ArgsObserverCallable
        : public ObserverCallableInterface
    {
    public:
        virtual void call( const typename Notificator<ID>::args_type & _args ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    template<uint32_t ID, class C, class M, class ... Args>
    class MethodObserverCallable
        : public ArgsObserverCallable<ID>
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
        void call( const typename Notificator<ID>::args_type & _args ) override
        {
            this->call_with_tuple( _args, std::make_integer_sequence<uint32_t, sizeof ... (Args)>() );
        }

        template<uint32_t ... I>
        void call_with_tuple( const typename Notificator<ID>::args_type & _args, std::integer_sequence<uint32_t, I...> )
        {
            (m_self->*m_method)(std::get<I>( _args )...);
        }

    protected:
        C * m_self;
        M m_method;
    };
    //////////////////////////////////////////////////////////////////////////
    template<uint32_t ID, class T>
    class GeneratorMethodObserverCallable;
    //////////////////////////////////////////////////////////////////////////
    template<uint32_t ID, class C, class ... P>
    class GeneratorMethodObserverCallable<ID, void (C:: *)(P...)>
        : public MethodObserverCallable<ID, C, void (C:: *)(P...), P ...>
    {
    public:
        GeneratorMethodObserverCallable( C * _self, void (C:: * _method)(P...) )
            : MethodObserverCallable<ID, C, void (C:: *)(P...), P ...>( _self, _method )
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<uint32_t ID, class C, class ... P>
    class GeneratorMethodObserverCallable<ID, void (C:: *)(P...) const>
        : public MethodObserverCallable<ID, C, void (C:: *)(P...) const, P ...>
    {
    public:
        GeneratorMethodObserverCallable( C * _self, void (C:: * _method)(P...) const )
            : MethodObserverCallable<ID, C, void (C:: *)(P...) const, P ...>( _self, _method )
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    class NotificationServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "NotificationService" )

    public:
        virtual void addObserver( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable, const Char * _doc ) = 0;
        virtual void removeObserver( uint32_t _id, const ObservablePtr & _observer ) = 0;

    public:
        template<uint32_t ID, class C, class M>
        void addObserverMethod( C * _self, M _method, const Char * _doc )
        {
            ObserverCallableInterfacePtr callable( new GeneratorMethodObserverCallable<ID, M>( _self, _method ) );

            this->addObserver( ID, ObservablePtr( _self ), callable, _doc );
        }

    public:
        typedef Lambda<void( const ObserverCallableInterfacePtr & )> LambdaObserver;
        virtual bool visitObservers( uint32_t _id, const LambdaObserver & _lambda ) = 0;

    public:
        template<uint32_t ID, class ... Args>
        bool notify( Args && ... _args )
        {
            bool successful = this->notify_tuple<ID>( std::make_tuple( std::forward<Args>( _args ) ... ) );

            return successful;
        }

    protected:
        template<uint32_t ID>
        bool notify_tuple( const typename Notificator<ID>::args_type & _args )
        {
            typedef ArgsObserverCallable<ID> args_observer_type;

            bool successful = this->visitObservers( ID, [&_args]( const ObserverCallableInterfacePtr & _observer )
            {
                args_observer_type * args_observer = _observer.getT<args_observer_type *>();

                args_observer->call( _args );
            } );

            return successful;
        }
    };
}
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_SERVICE()\
    ((Mengine::NotificationServiceInterface*)SERVICE_GET(Mengine::NotificationServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_ADDOBSERVERMETHOD( ID, Observer, Method )\
    NOTIFICATION_SERVICE()->addObserverMethod<ID>( Observer, Method, MENGINE_DOCUMENT_FUNCTION )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_REMOVEOBSERVER( ID, Observer )\
    NOTIFICATION_SERVICE()->removeObserver( ID, Observer )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_REMOVEOBSERVER_THIS( ID )\
    NOTIFICATION_SERVICE()->removeObserver( ID, Mengine::ObservablePtr(this) )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_NOTIFY( ID, ... )\
    NOTIFICATION_SERVICE()->notify<ID>( __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////
