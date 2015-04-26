#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#	include "Kernel/EventEnum.h"

#   include "stdex/stl_map.h"

#	include "pybind/types.hpp"
#	include "pybind/object.hpp"

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
		bool registerEvent( EEventName _event, const char * _method, PyObject * _dict, bool * _exist = nullptr );
		bool registerEventMethod( EEventName _event, const char * _method, PyObject * _module, bool * _exist = nullptr );

		PyObject * getEvent( EEventName _event ) const;
		bool hasEvent( EEventName _event ) const;

	protected:
		void unregisterEvents_();

	protected:
		PyObject * getEvent_( const char * _method, PyObject * _dict ) const;
		PyObject * getEventMethod_( const char * _method, PyObject * _module ) const;
		void insertEvent_( EEventName _event, PyObject * _cb );
		void removeEvent_( EEventName _event );

	private:
		typedef stdex::map<EEventName, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
	//////////////////////////////////////////////////////////////////////////
	class EventableCallOperator
	{
	public:
		EventableCallOperator( ServiceProviderInterface * _serviceProvider, EEventName _event, PyObject * _pyevent );

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
	//////////////////////////////////////////////////////////////////////////
	class EventableAskOperator
	{
	public:
		EventableAskOperator( ServiceProviderInterface * _serviceProvider, EEventName _event, PyObject * _pyevent );

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
	for( PyObject * EVENT_CALL_pyevent = Self->getEvent(Event); EVENT_CALL_pyevent != nullptr; EVENT_CALL_pyevent = nullptr )\
	EventableCallOperator(serviceProvider, Event, EVENT_CALL_pyevent).getEvent()

#	define EVENTABLE_ASK(serviceProvider, Self, Event, Value)\
	for( bool EVENTABLE_ASK_self = false; Self != nullptr && EVENTABLE_ASK_self == false; EVENTABLE_ASK_self = true )\
	for( PyObject * EVENTABLE_ASK_pyevent = Self->getEvent(Event); EVENTABLE_ASK_pyevent != nullptr; EVENTABLE_ASK_pyevent = nullptr )\
	Value = EventableAskOperator(serviceProvider, Event, EVENTABLE_ASK_pyevent).getEvent()

