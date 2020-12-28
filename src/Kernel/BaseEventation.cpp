#include "BaseEventation.h"

#include "Kernel/Assertion.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseEventation::BaseEventation() noexcept
        : m_receiverMask( 0 )
        , m_receiversMask( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseEventation::~BaseEventation() noexcept
    {
        //MENGINE_ASSERTION_FATAL( m_receiver == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseEventation::setReceiver( uint64_t _receiverMask, const EventReceiverInterfacePtr & _receiver )
    {
        m_receiverMask = _receiverMask;

        m_receiver = _receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    const EventReceiverInterfacePtr & BaseEventation::getReceiver() const
    {
        return m_receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    EventReceiverInterfacePtr BaseEventation::addEventReceiver( uint32_t _event, const EventReceiverInterfacePtr & _receiver )
    {
        MENGINE_ASSERTION_FATAL( _event < (sizeof( m_receiversMask ) * 8 - 1) );
        MENGINE_ASSERTION_FATAL( (m_receiverMask & (1ULL << _event)) == 0 );

        if( (m_receiversMask & (1ULL << _event)) != 0 )
        {
            VectorEventReceivers::iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), [_event]( const EventReceiverDesc & _desc )
            {
                return _desc.event == _event;
            } );

            MENGINE_ASSERTION_FATAL( it_found != m_receivers.end() );

            EventReceiverDesc & desc = *it_found;

            EventReceiverInterfacePtr oldreceiver = desc.receiver;
            desc.receiver = _receiver;

            return oldreceiver;
        }

        EventReceiverDesc desc;
        desc.event = _event;
        desc.receiver = _receiver;

        m_receivers.emplace_back( desc );

        m_receiversMask |= (1ULL << _event);

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    EventReceiverInterfacePtr BaseEventation::removeEventReceiver( uint32_t _event )
    {
        MENGINE_ASSERTION_FATAL( _event < (sizeof( m_receiversMask ) * 8 - 1) );
        MENGINE_ASSERTION_FATAL( (m_receiverMask & (1ULL << _event)) == 0 );

        VectorEventReceivers::iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), [_event]( const EventReceiverDesc & _desc )
        {
            return _desc.event == _event;
        } );

        if( it_found == m_receivers.end() )
        {
            return nullptr;
        }

        const EventReceiverDesc & desc = *it_found;

        EventReceiverInterfacePtr delreceiver = desc.receiver;

        m_receivers.erase( it_found );

        m_receiversMask &= ~(1ULL << _event);

        return delreceiver;
    }
    //////////////////////////////////////////////////////////////////////////
    const EventReceiverInterfacePtr & BaseEventation::getEventReciever( uint32_t _event ) const
    {
        MENGINE_ASSERTION_FATAL( _event < (sizeof( m_receiversMask ) * 8 - 1) );

        if( (m_receiverMask & (1ULL << _event)) != 0 )
        {
            return m_receiver;
        }

        VectorEventReceivers::const_iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), [_event]( const EventReceiverDesc & _desc )
        {
            return _desc.event == _event;
        } );

        if( it_found == m_receivers.end() )
        {
            return EventReceiverInterfacePtr::none();
        }

        const EventReceiverInterfacePtr & receiver = it_found->receiver;

        return receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseEventation::hasEventReceiver( uint32_t _event ) const
    {
        MENGINE_ASSERTION_FATAL( _event < (sizeof( m_receiversMask ) * 8 - 1) );

        return ((m_receiverMask | m_receiversMask) & (1ULL << _event)) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseEventation::removeEvents() noexcept
    {
        m_receiver = nullptr;

        m_receivers.clear();
        m_receiversMask = 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
