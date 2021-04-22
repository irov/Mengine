#pragma once

#include "Interface/EventReceiverInterface.h"

#ifdef MENGINE_DEBUG
#   include <type_traits>
#   include <stdexcept>
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint64_t EVENTATION_RECEIVER_MASK_FULL = ~0ULL;
    //////////////////////////////////////////////////////////////////////////
    class Eventable;
    //////////////////////////////////////////////////////////////////////////
    class EventationInterface
        : public Interface
    {
    public:
        virtual Eventable * getEventable() = 0;

    public:
        virtual void setReceiver( uint64_t _receiverMask, const EventReceiverInterfacePtr & _receiver ) = 0;
        virtual const EventReceiverInterfacePtr & getReceiver() const = 0;

    public:
        virtual EventReceiverInterfacePtr addEventReceiver( uint32_t _event, const EventReceiverInterfacePtr & _receiver ) = 0;
        virtual EventReceiverInterfacePtr removeEventReceiver( uint32_t _event ) = 0;

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

            EventReceiverInterface * r = reciever.get();

#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<EventReceiverInterface, std::remove_pointer_t<T>>, "static event reciever cast use on non 'EventReceiverInterface' type");

            if( r == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<T>(r) == nullptr )
            {
                throw std::runtime_error( "static event reciever cast" );
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