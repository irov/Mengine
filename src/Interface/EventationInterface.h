#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EventReceiver
        : public Factorable
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EventReceiver> EventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class EventationInterface
        : public Mixin
    {
    public:
        virtual bool registerEventReceiver( uint32_t _event, const EventReceiverPtr & _receiver ) = 0;
        virtual void removeEventReceiver( uint32_t _event ) = 0;

    public:
        virtual const EventReceiverPtr & getEventReciever( uint32_t _event ) const = 0;
        virtual bool hasEventReceiver( uint32_t _event ) const = 0;

    public:
        virtual void removeEvents() = 0;

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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EventationInterface> EventationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}