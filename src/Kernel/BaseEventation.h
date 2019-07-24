#pragma once

#include "Interface/EventationInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseEventation
        : public EventationInterface
    {
    public:
        BaseEventation() noexcept;
        ~BaseEventation() noexcept override;

    public:
        void setReceiver( uint64_t _receiverMask, const EventReceiverInterfacePtr & _receiver ) override;
        const EventReceiverInterfacePtr & getReceiver() const override;

    public:
        bool addEventReceiver( uint32_t _event, const EventReceiverInterfacePtr & _receiver ) override;
        void removeEventReceiver( uint32_t _event ) override;

    public:
        const EventReceiverInterfacePtr & getEventReciever( uint32_t _event ) const override;
        bool hasEventReceiver( uint32_t _event ) const override;

    public:
        void removeEvents() noexcept override;

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
    typedef IntrusivePtr<BaseEventation> BaseEventPtr;
    //////////////////////////////////////////////////////////////////////////
}



