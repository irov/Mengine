#pragma once

#include "Interface/EventReceiverInterface.h"

#include "Config/Lambda.h"

#if defined(MENGINE_DEBUG)
#   include "Config/TypeTraits.h"
#   include "Config/DynamicCast.h"
#   include "Config/StdException.h"
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
        virtual const EventReceiverInterfacePtr & getEventReceiver( uint32_t _event ) const = 0;
        virtual bool hasEventReceiver( uint32_t _event ) const = 0;

    public:
        typedef Lambda<void( uint32_t _event, const EventReceiverInterfacePtr & _receiver )> LambdaEventReceivers;
        virtual void foreachEventReceivers( const LambdaEventReceivers & _lambda ) const = 0;
        virtual void removeEvents() noexcept = 0;

    public:
        template<class T>
        T getEventReceiverT( uint32_t _event ) const
        {
            const EventReceiverInterfacePtr & receiver = this->getEventReceiver( _event );

            EventReceiverInterface * r = receiver.get();

#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<EventReceiverInterface, T>, "static event receiver cast use on non 'EventReceiverInterface' type");

            if( r == nullptr )
            {
                return nullptr;
            }

            if( Helper::dynamicCast<T>( r ) == nullptr )
            {
                throw StdException::runtime_error( "static event receiver cast" );
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