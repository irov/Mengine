#pragma once

#include "Interface/Interface.h"

namespace Mengine
{
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
        virtual bool registerEventReceiver( uint32_t _event, const EventReceiverInterfacePtr & _receiver ) = 0;
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