#pragma once

#include "Config/Lambda.h"

#include "Interface/ServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factorable.h"
#include "Kernel/Observable.h"
#include "Kernel/Exception.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/Tuple.h"

#include <type_traits>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ExceptionNotificationFailed
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class ObserverCallableInterface
        : public Factorable
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ObserverCallableInterface> ObserverCallableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    template<NotificatorId ID>
    class ArgsObserverCallable
        : public ObserverCallableInterface
    {
    public:
        virtual void call( typename Notificator<ID>::args_type && _args ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    template<NotificatorId ID, class C, class M, class ... Args>
    class MethodObserverCallable
        : public ArgsObserverCallable<ID>
    {
    public:
        MethodObserverCallable( C * _self, M _method )
            : m_self( _self )
            , m_method( _method )
        {
        }

        ~MethodObserverCallable() override
        {
        }

    protected:
        void call( typename Notificator<ID>::args_type && _args ) override
        {
            this->call_with_tuple( std::forward<typename Notificator<ID>::args_type>( _args ), std::make_integer_sequence<NotificatorId, sizeof ... (Args)>() );
        }

        template<NotificatorId ... I>
        void call_with_tuple( typename Notificator<ID>::args_type && _args, std::integer_sequence<NotificatorId, I...> )
        {
            (m_self->*m_method)(std::get<I>( std::forward<typename Notificator<ID>::args_type>( _args ) )...);
        }

    protected:        
#if defined(MENGINE_DEBUG) //avoid warning C4324
        C * m_self;
        M m_method;
#else
        M m_method;
        C * m_self;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    template<NotificatorId ID, class L>
    class LambdaObserverCallable
        : public ArgsObserverCallable<ID>
    {
    public:
        LambdaObserverCallable( const L & _lambda )
            : m_lambda( _lambda )
        {
        }

        ~LambdaObserverCallable() override
        {
        }

    protected:
        void call( typename Notificator<ID>::args_type && _args ) override
        {
            std::apply( m_lambda, std::forward<typename Notificator<ID>::args_type && >( _args ) );
        }

    protected:
        L m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    template<NotificatorId ID, class T>
    class GeneratorMethodObserverCallable;
    //////////////////////////////////////////////////////////////////////////
    template<NotificatorId ID, class C, class ... P>
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
    template<NotificatorId ID, class C, class ... P>
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
        virtual void addObserver( NotificatorId _id, Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeObserver( NotificatorId _id, Observable * _observer ) = 0;

    public:
        template<NotificatorId ID, class C, class L>
        void addObserverLambda( C * _self, const L & _lambda, const DocumentInterfacePtr & _doc )
        {
            ObserverCallableInterfacePtr callable( Helper::makeFactorableUnique<LambdaObserverCallable<ID, L>>( _doc, _lambda ) );

            this->addObserver( ID, _self, callable, _doc );
        }

    public:
        template<NotificatorId ID, class C, class M>
        void addObserverMethod( C * _self, M _method, const DocumentInterfacePtr & _doc )
        {
            ObserverCallableInterfacePtr callable( Helper::makeFactorableUnique<GeneratorMethodObserverCallable<ID, M>>( _doc, _self, _method ) );

            this->addObserver( ID, _self, callable, _doc );
        }

    public:
        virtual bool hasObserver( Observable * _observer ) const = 0;

    public:
        typedef Lambda<void( const ObserverCallableInterfacePtr & )> LambdaObserver;
        virtual bool foreachObservers( NotificatorId _id, const LambdaObserver & _lambda ) = 0;

    public:
        template<NotificatorId ID, class ... Args>
        bool notify( Args && ... _args )
        {
            bool successful = this->notify_tuple<ID>( std::forward_as_tuple( std::forward<Args>( _args ) ... ) );

            return successful;
        }

    protected:
        template<NotificatorId ID>
        bool notify_tuple( typename Notificator<ID>::args_type && _args )
        {
            typedef ArgsObserverCallable<ID> args_observer_type;

            bool successful = this->foreachObservers( ID, [args = std::forward<typename Notificator<ID>::args_type>( _args )]( const ObserverCallableInterfacePtr & _observer ) mutable
            {
                args_observer_type * args_observer = _observer.getT<args_observer_type *>();

                args_observer->call( std::forward<typename Notificator<ID>::args_type>( args ) );
            } );

            return successful;
        }
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_SERVICE()\
    ((Mengine::NotificationServiceInterface*)SERVICE_GET(Mengine::NotificationServiceInterface))
//////////////////////////////////////////////////////////////////////////
