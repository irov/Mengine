#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#   include "stdex/stl_map.h"

#	include "pybind/types.hpp"
#	include "pybind/object.hpp"
#	include "pybind/dict.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class Eventable
	{
	public:
		Eventable();
		~Eventable();

	public:
		bool registerEvent( uint32_t _event, const char * _method, const pybind::dict & _dict );
		bool registerEventMethod( uint32_t _event, const char * _method, const pybind::object & _module );

		pybind::object getEvent( uint32_t _event ) const;
		bool hasEvent( uint32_t _event ) const;

	protected:
		void unregisterEvents_();

	protected:
		pybind::object getEvent_( const pybind::dict & _dict, const char * _method ) const;
		pybind::object getEventMethod_( const pybind::object & _module, const char * _method ) const;
		void insertEvent_( uint32_t _event, const pybind::object & _cb );
		void removeEvent_( uint32_t _event );

	private:
		typedef stdex::map<uint32_t, pybind::object> TMapEvent;
		TMapEvent m_mapEvent;

		uint32_t m_flag;
	};
	//////////////////////////////////////////////////////////////////////////
	class EventableCallOperator
	{
	public:
		EventableCallOperator( ServiceProviderInterface * _serviceProvider, uint32_t _event, const pybind::object & _pyevent )
			: m_serviceProvider( _serviceProvider )
			, m_event( _event )
			, m_pyevent( _pyevent )
		{
		}

	public:
		const pybind::object & getEvent() const
		{
			return m_pyevent;
		}

	protected:
        ServiceProviderInterface * m_serviceProvider;

		uint32_t m_event;
		pybind::object m_pyevent;		
	};
}

#	define EVENTABLE_CALL(serviceProvider, Self, Event)\
	for( bool EVENTABLE_CALL_self = false; Self != nullptr && EVENTABLE_CALL_self == false; EVENTABLE_CALL_self = true )\
	for( const pybind::object & EVENTABLE_CALL_pyevent = Self->getEvent(Event); EVENTABLE_CALL_pyevent.is_invalid() == false && EVENTABLE_CALL_self == false; EVENTABLE_CALL_self = true )\
	EventableCallOperator(serviceProvider, Event, EVENTABLE_CALL_pyevent ).getEvent().call

#	define EVENTABLE_ASK(serviceProvider, Self, Event, Value)\
	for( bool EVENTABLE_CALL_self = false; Self != nullptr && EVENTABLE_CALL_self == false; EVENTABLE_CALL_self = true )\
	for( const pybind::object & EVENTABLE_CALL_pyevent = Self->getEvent(Event); EVENTABLE_CALL_pyevent.is_invalid() == false && EVENTABLE_CALL_self == false; EVENTABLE_CALL_self = true )\
	Value = EventableCallOperator(serviceProvider, Event, EVENTABLE_CALL_pyevent).getEvent().call

