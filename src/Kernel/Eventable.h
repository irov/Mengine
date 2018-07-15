#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/EventReceiver.h"
#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Eventable
        : public Mixin
    {
    public:
        Eventable();
        ~Eventable() override;

    public:
        bool registerEventReceiver( uint32_t _event, const EventReceiverPtr & _receiver );
        void removeEventReceiver( uint32_t _event );

    public:
        const EventReceiverPtr & getEventReciever( uint32_t _event ) const;
        bool hasEventReceiver( uint32_t _event ) const;

    public:
        void removeEvents();

    public:
        template<class T>
        T getEventRecieverT( uint32_t _event ) const
        {
            const EventReceiverPtr & reciever = this->getEventReciever( _event );

            if( reciever == nullptr )
            {
                return nullptr;
            }

            EventReceiver * r = reciever.get();

#ifndef NDEBUG            
            if( dynamic_cast<T>(r) == nullptr )
            {
                throw;
            }
#endif

            T t = static_cast<T>(r);

            return t;
        }

    protected:
        struct EventReceiverDesc
        {
            uint32_t event;
            EventReceiverPtr receiver;
        };

        typedef Vector<EventReceiverDesc> TVectorEventReceivers;
        TVectorEventReceivers m_receivers;

        uint64_t m_flag;

    private:
        class FEventReciver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Eventable> EventablePtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        static typename T::EventReceiverType * getThisEventRecieverT( T * _self, uint32_t _event )
        {
            typedef typename T::EventReceiverType * T_EventReceiverType;

            T_EventReceiverType reciever = _self->template getEventRecieverT<T_EventReceiverType>( _event );

            return reciever;
        }

        template<class T>
        static typename T::EventReceiverType * getThisEventRecieverT( const IntrusivePtr<T> & _self, uint32_t _event )
        {
            typedef typename T::EventReceiverType * T_EventReceiverType;

            T_EventReceiverType reciever = _self->template getEventRecieverT<T_EventReceiverType>( _event );

            return reciever;
        }

        template<class T>
        static T * getThisEventReciever( Eventable * _self, uint32_t _event )
        {
            T * reciever = _self->getEventRecieverT<T *>( _event );

            return reciever;
        }

        template<class T>
        static T * getThisEventReciever( const EventablePtr & _self, uint32_t _event )
        {
            T * reciever = _self->getEventRecieverT<T>( _event );

            return reciever;
        }
    }
}

#   define EVENT_RECEIVER(Type)\
public:\
    typedef Type EventReceiverType

#   define EVENTABLE_METHODR(Self, Event, R)\
    Self == nullptr ? R : Self->hasEventReceiver( Event ) == false ? R : Helper::getThisEventRecieverT( Self, Event )

#   define EVENTABLE_METHOD(Self, Event)\
    EVENTABLE_METHODR(Self, Event, ((void)0))

#   define EVENTABLE_METHODRS(Self, Event, R)\
    Self->hasEventReceiver( Event ) == false ? R : Helper::getThisEventRecieverT( Self, Event )

#   define EVENTABLE_METHODRT(Self, Event, R, Type)\
    Self == nullptr ? R : Self->hasEventReceiver( Event ) == false ? R : Helper::getThisEventReciever<Type>( Self, Event )

#   define EVENTABLE_METHODT(Self, Event, Type)\
    EVENTABLE_METHODRT(Self, Event, ((void)0), Type)

#   define EVENTABLE_METHODRTS(Self, Event, R, Type)\
    Self->hasEventReceiver( Event ) == false ? R : Helper::getThisEventReciever<Type>( Self, Event )


