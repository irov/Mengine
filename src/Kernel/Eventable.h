#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#	include "Kernel/EventEnum.h"

#   include "stdex/stl_map.h"

#	include "pybind/types.hpp"
#	include "pybind/object.hpp"
#	include "pybind/dict.hpp"

#	include <string>
#	include <stdarg.h>

namespace Menge
{
	class Scriptable;
	//////////////////////////////////////////////////////////////////////////
	class Eventable
	{
	public:
		Eventable();
		~Eventable();

	public:
		bool registerEvent( EEventName _event, const char * _method, const pybind::dict & _dict, bool * _exist = nullptr );
		bool registerEventMethod( EEventName _event, const char * _method, const pybind::object & _module, bool * _exist = nullptr );

		pybind::object getEvent( EEventName _event ) const;
		bool hasEvent( EEventName _event ) const;

	protected:
		void unregisterEvents_();

	protected:
		pybind::object getEvent_( const pybind::dict & _dict, const char * _method ) const;
		pybind::object getEventMethod_( const pybind::object & _module, const char * _method ) const;
		void insertEvent_( EEventName _event, const pybind::object & _cb );
		void removeEvent_( EEventName _event );

	private:
		typedef stdex::map<EEventName, pybind::object> TMapEvent;
		TMapEvent m_mapEvent;
	};
	//////////////////////////////////////////////////////////////////////////
	class EventableCallOperator
	{
	public:
		EventableCallOperator::EventableCallOperator( ServiceProviderInterface * _serviceProvider, EEventName _event, const pybind::object & _pyevent )
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

		EEventName m_event;
		pybind::object m_pyevent;		
	};
}

#	define EVENTABLE_CALL(serviceProvider, Self, Event)\
	for( bool EVENTABLE_CALL_self = false; Self != nullptr && EVENTABLE_CALL_self == false; EVENTABLE_CALL_self = true )\
	for( const pybind::object & EVENTABLE_CALL_pyevent = Self->getEvent(Event); EVENTABLE_CALL_pyevent.is_invalid() == false && EVENTABLE_CALL_self == false; EVENTABLE_CALL_self = true )\
	EventableCallOperator(serviceProvider, Event, EVENTABLE_CALL_pyevent ).getEvent()

#	define EVENTABLE_ASK(serviceProvider, Self, Event, Value)\
	for( bool EVENTABLE_CALL_self = false; Self != nullptr && EVENTABLE_CALL_self == false; EVENTABLE_CALL_self = true )\
	for( const pybind::object & EVENTABLE_CALL_pyevent = Self->getEvent(Event); EVENTABLE_CALL_pyevent.is_invalid() == false && EVENTABLE_CALL_self == false; EVENTABLE_CALL_self = true )\
	Value = EventableCallOperator(serviceProvider, Event, EVENTABLE_CALL_pyevent).getEvent()

