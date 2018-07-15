#include "BaseEvent.h"

#include <algorithm>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	BaseEvent::BaseEvent()
		: m_flag(0)
	{
    }
	//////////////////////////////////////////////////////////////////////////
	BaseEvent::~BaseEvent()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    class BaseEvent::FEventReciver
    {
    public:
        FEventReciver( uint32_t _event )
            : m_event( _event )
        {
        }

    public:
        bool operator() ( const EventReceiverDesc & _desc ) const
        {
            return _desc.event == m_event;
        }

    protected:
        uint32_t m_event;
    };
    //////////////////////////////////////////////////////////////////////////
    bool BaseEvent::registerEventReceiver( uint32_t _event, const EventReceiverPtr & _receiver )
    {
#ifndef NDEBUG
        if( _event >= (sizeof( m_flag ) * 8 - 1) )
        {
            throw;
        }
#endif

        TVectorEventReceivers::iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), FEventReciver( _event ) );

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
    void BaseEvent::removeEventReceiver( uint32_t _event )
    {
#ifndef NDEBUG
        if( _event >= (sizeof( m_flag ) * 8) )
        {
            throw;
        }
#endif

        TVectorEventReceivers::iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), FEventReciver( _event ) );

        if( it_found == m_receivers.end() )
        {
            return;
        }

        m_receivers.erase( it_found );

        m_flag &= ~(1L << _event);
    }
    //////////////////////////////////////////////////////////////////////////
    const EventReceiverPtr & BaseEvent::getEventReciever( uint32_t _event ) const
    {
#ifndef NDEBUG
        if( _event >= (sizeof( m_flag ) * 8) )
        {
            throw;
        }
#endif

        TVectorEventReceivers::const_iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), FEventReciver( _event ) );

        if( it_found == m_receivers.end() )
        {
            return EventReceiverPtr::none();
        }
        
        const EventReceiverPtr & receiver = it_found->receiver;

        return receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseEvent::hasEventReceiver( uint32_t _event ) const
    {
#ifndef NDEBUG
        if( _event >= (sizeof( m_flag ) * 8) )
        {
            throw;
        }
#endif

        return (m_flag & (1ULL << _event)) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseEvent::removeEvents()
    {
        m_receivers.clear();
        m_flag = 0;
    }
}
