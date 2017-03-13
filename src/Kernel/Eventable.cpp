#	include "Eventable.h"

#   include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t max_event_id = (sizeof(event_t) * 8 - 1);
	//////////////////////////////////////////////////////////////////////////
	Eventable::Eventable()
		: m_flag(0)
	{
    }
	//////////////////////////////////////////////////////////////////////////
	Eventable::~Eventable()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    class Eventable::FEventReciver
    {
    public:
        FEventReciver(event_t _event )
            : m_event( _event )
        {
        }

    public:
        bool operator() ( const EventReceiverDesc & _desc ) const
        {
            return _desc.event == m_event;
        }

    protected:
		event_t m_event;
    };
    //////////////////////////////////////////////////////////////////////////
    bool Eventable::registerEventReceiver(event_t _event, const EventReceiverPtr & _receiver )
    {
#   ifdef _DEBUG
        if( _event >= max_event_id)
        {
            throw;
        }
#   endif

        TMapEventReceivers::iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), FEventReciver( _event ) );

        if( it_found == m_receivers.end() )
        {
            EventReceiverDesc desc;
            desc.event = _event;
            desc.receiver = _receiver;

            m_receivers.push_back( desc );

            m_flag |= (1 << _event);
        }
        else
        {
            it_found->receiver = _receiver;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Eventable::removeEventReceiver(event_t _event )
    {
#   ifdef _DEBUG
        if( _event >= max_event_id)
        {
            throw;
        }
#   endif

        TMapEventReceivers::iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), FEventReciver( _event ) );

        if( it_found == m_receivers.end() )
        {
            return;
        }

        m_receivers.erase( it_found );

        m_flag &= ~(1 << _event);
    }
    //////////////////////////////////////////////////////////////////////////
    const EventReceiverPtr & Eventable::getEventReciever(event_t _event ) const
    {
#   ifdef _DEBUG
        if( _event >= max_event_id)
        {
            throw;
        }
#   endif

        TMapEventReceivers::const_iterator it_found = std::find_if( m_receivers.begin(), m_receivers.end(), FEventReciver( _event ) );

        if( it_found == m_receivers.end() )
        {
            return EventReceiverPtr::none();
        }
        
        const EventReceiverPtr & receiver = it_found->receiver;

        return receiver;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Eventable::hasEventReceiver(event_t _event ) const
    {
#   ifdef _DEBUG
        if( _event >= max_event_id)
        {
            throw;
        }
#   endif

        return (m_flag & (1 << _event)) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void Eventable::removeEvents()
    {
        m_receivers.clear();
        m_flag = 0;
    }
}
