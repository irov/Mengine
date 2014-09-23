#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#	include "Kernel/EventEnum.h"

#   include "stdex/binary_vector.h"

#	include "pybind/types.hpp"

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
		void removeEvent_( EEventName _event );

	private:
		typedef stdex::binary_vector<EEventName, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
	//////////////////////////////////////////////////////////////////////////
	class EventableCallOperator
	{
	public:
		EventableCallOperator( ServiceProviderInterface * _serviceProvider, EEventName _event, PyObject * _pyevent );

	public:
		bool operator () ( const char * _format, ... );

	protected:
        ServiceProviderInterface * m_serviceProvider;

		EEventName m_event;
		PyObject * m_pyevent;		
	};
	//////////////////////////////////////////////////////////////////////////
	class EventableAskOperator
	{
	public:
		EventableAskOperator( ServiceProviderInterface * _serviceProvider, EEventName _event, PyObject * _pyevent );

	public:
		bool operator () ( bool & _value, bool _default, const char * _format, ... ) const;
		bool operator () ( uint32_t & _value, uint32_t _default, const char * _format, ... ) const;
		bool operator () ( Scriptable *& _value, Scriptable * _default, const char * _format, ... ) const;
		bool operator () ( PyObject *& _value, PyObject * _default, const char * _format, ... ) const;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		EEventName m_event;
		PyObject * m_pyevent;		
	};
}

#	define EVENTABLE_CALL(serviceProvider, Self, Event)\
	for( bool EVENTABLE_CALL_self = false; Self != nullptr && EVENTABLE_CALL_self == false; EVENTABLE_CALL_self = true )\
	for( PyObject * EVENT_CALL_pyevent = Self->getEvent(Event); EVENT_CALL_pyevent != nullptr; EVENT_CALL_pyevent = nullptr )\
	EventableCallOperator(serviceProvider, Event, EVENT_CALL_pyevent)

#	define EVENTABLE_ASK(serviceProvider, Self, Event)\
	for( bool EVENTABLE_ASK_self = false; Self != nullptr && EVENTABLE_ASK_self == false; EVENTABLE_ASK_self = true )\
	for( PyObject * EVENT_ASK_pyevent = Self->getEvent(Event); EVENT_ASK_pyevent != nullptr; EVENT_ASK_pyevent = nullptr )\
	EventableAskOperator(serviceProvider, Event, EVENT_ASK_pyevent)

