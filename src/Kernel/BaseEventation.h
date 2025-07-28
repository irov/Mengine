#pragma once

#include "Interface/EventationInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseEventation
        : public EventationInterface
    {
    public:
        BaseEventation();
        ~BaseEventation() override;

    public:
        void setReceiver( uint64_t _receiverMask, const EventReceiverInterfacePtr & _receiver ) override final;
        const EventReceiverInterfacePtr & getReceiver() const override final;

    public:
        EventReceiverInterfacePtr addEventReceiver( uint32_t _event, const EventReceiverInterfacePtr & _receiver ) override final;
        EventReceiverInterfacePtr removeEventReceiver( uint32_t _event ) override final;

    public:
        const EventReceiverInterfacePtr & getEventReceiver( uint32_t _event ) const override final;
        bool hasEventReceiver( uint32_t _event ) const override final;

    public:
        void foreachEventReceivers( const LambdaEventReceivers & _lambda ) const override final;
        void removeEvents() override final;

    protected:
        EventReceiverInterfacePtr m_receiver;
        uint64_t m_receiverMask;

        struct EventReceiverDesc
        {
            uint32_t event;
            EventReceiverInterfacePtr receiver;
        };

        typedef Vector<EventReceiverDesc> VectorEventReceivers;
        VectorEventReceivers m_receivers;

        uint64_t m_receiversMask;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BaseEventation, EventationInterface> BaseEventPtr;
    //////////////////////////////////////////////////////////////////////////
}