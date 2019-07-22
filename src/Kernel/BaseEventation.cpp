#include "BaseEventation.h"

#include "Kernel/Assertion.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseEventation::BaseEventation() noexcept
        : m_flag( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseEventation::~BaseEventation() noexcept
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseEventation::addEventReceiver( uint32_t _event, const EventReceiverInterfacePtr & _receiver )
    {
        MENGINE_ASSERTION_FATAL( _event < (sizeof( m_flag ) * 8 - 1) );

        VectorEventReceivers::iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), [_event]( const EventReceiverDesc & _desc )
        {
            return _desc.event == _event;
        } );

        if( it_found == m_receivers.end() )
        {
            EventReceiverDesc desc;
            desc.event = _event;
            desc.receiver = _receiver;

            m_receivers.emplace_back( desc );

            m_flag |= (1ULL << _event);
        }
        else
        {
            it_found->receiver = _receiver;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseEventation::removeEventReceiver( uint32_t _event )
    {
        MENGINE_ASSERTION_FATAL( _event < (sizeof( m_flag ) * 8 - 1) );

        VectorEventReceivers::iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), [_event]( const EventReceiverDesc & _desc )
        {
            return _desc.event == _event;
        } );

        if( it_found == m_receivers.end() )
        {
            return;
        }

        m_receivers.erase( it_found );

        m_flag &= ~(1ULL << _event);
    }
    //////////////////////////////////////////////////////////////////////////
    const EventReceiverInterfacePtr & BaseEventation::getEventReciever( uint32_t _event ) const
    {
        MENGINE_ASSERTION_FATAL( _event < (sizeof( m_flag ) * 8 - 1) );

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
        MENGINE_ASSERTION_FATAL( _event < (sizeof( m_flag ) * 8 - 1) );

        return (m_flag & (1ULL << _event)) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseEventation::removeEvents() noexcept
    {
        m_receivers.clear();
        m_flag = 0;
    }
}
