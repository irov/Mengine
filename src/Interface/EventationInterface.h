#pragma once

#include "Interface/Interface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const uint64_t RECEIVER_MASK_FULL = ~0ULL;
    //////////////////////////////////////////////////////////////////////////
    class EventReceiverInterface
        : public Interface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EventReceiverInterface> EventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class EventationInterface
        : public Interface
    {
    public:
        virtual void setReceiver( uint64_t _receiverMask, const EventReceiverInterfacePtr & _receiver ) = 0;
        virtual const EventReceiverInterfacePtr & getReceiver() const = 0;

    public:
        virtual bool addEventReceiver( uint32_t _event, const EventReceiverInterfacePtr & _receiver ) = 0;
        virtual void removeEventReceiver( uint32_t _event ) = 0;

    public:
        virtual const EventReceiverInterfacePtr & getEventReciever( uint32_t _event ) const = 0;
        virtual bool hasEventReceiver( uint32_t _event ) const = 0;

    public:
        virtual void removeEvents() noexcept = 0;

    public:
        template<class T>
        T getEventRecieverT( uint32_t _event ) const
        {
            const EventReceiverInterfacePtr & reciever = this->getEventReciever( _event );

            if( reciever == nullptr )
            {
                return nullptr;
            }

            EventReceiverInterface * r = reciever.get();

#ifdef MENGINE_DEBUG
            if( dynamic_cast<T>(r) == nullptr )
            {
                throw;
            }
#endif

            T t = static_cast<T>(r);

            return t;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EventationInterface> EventationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define EVENTATION_TYPEID(Type, Event)\
    typedef Type EventationTypeid_##Event;
//////////////////////////////////////////////////////////////////////////
#define EVENTATION_GETTYPE(Event)\
    EventationTypeid_##Event
//////////////////////////////////////////////////////////////////////////