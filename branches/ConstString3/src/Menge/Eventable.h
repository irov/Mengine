#	pragma once

#	include <map>
#	include <string>

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

#	include "EventEnum.h"

#	include "pybind/types.hpp"

namespace Menge
{
	class Scriptable;

	class Eventable
	{
	public:
		Eventable();
		~Eventable();

	public:
		bool registerEvent( EEventName _event, const char * _method, PyObject * _module, bool * _exist = NULL );
		PyObject * getEvent( EEventName _event ) const;
		bool hasEvent( EEventName _event ) const;

	public:
		void callEvent( EEventName _event, const char * _format, ... ) const;
		void callEventDeferred( EEventName _event, const char * _format, ... ) const;

	public:
		void callPyEventVaList( EEventName _event, PyObject * _pyevent, const char * _format, const va_list & _va ) const;

	public:
		bool askEvent( bool & _result, EEventName _event, const char * _format, ... ) const;
		bool askEvent( size_t & _result, EEventName _event, const char * _format, ... ) const;
		bool askEvent( Scriptable *& _result, EEventName _event, const char * _format, ... ) const;
		bool askEvent( PyObject *& _result, EEventName _event, const char * _format, ... ) const;

	public:
		bool askPyEventVaList( bool & _result, EEventName _event, PyObject * _pyevent, const char * _format, const va_list & _va ) const;
		bool askPyEventVaList( size_t & _result, EEventName _event, PyObject * _pyevent, const char * _format, const va_list & _va ) const;
		bool askPyEventVaList( Scriptable *& _result, EEventName _event, PyObject * _pyevent, const char * _format, const va_list & _va ) const;
		bool askPyEventVaList( PyObject *& _result, EEventName _event, PyObject * _pyevent, const char * _format, const va_list & _va ) const;

	public:
		void removeAllEvent();

	protected:
		PyObject * getEvent_( const char * _method, PyObject * _dict ) const;
		void removeEvent_( EEventName _event );

	protected:
		virtual void _beginEvent( EEventName _event ) const;
		virtual void _endEvent( EEventName _event ) const;

	private:
		typedef std::map<EEventName, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};

	class EventableCallOperator
	{
	public:
		EventableCallOperator( const Eventable * _self, EEventName _event, PyObject * _pyevent )
			: m_self(_self)
			, m_event(_event)
			, m_pyevent(_pyevent)
		{
		}

	public:
		void operator () ( const char * _format, ... );

	protected:
		const Eventable * m_self;

		EEventName m_event;
		PyObject * m_pyevent;		
	};

	class EventableAskOperator
	{
	public:
		EventableAskOperator( const Eventable * _self, EEventName _event, PyObject * _pyevent )
			: m_self(_self)
			, m_event(_event)
			, m_pyevent(_pyevent)
		{
		}

	public:
		bool operator () ( bool & _value, bool _default, const char * _format, ... ) const;
		bool operator () ( size_t & _value, size_t _default, const char * _format, ... ) const;
		bool operator () ( Scriptable *& _value, Scriptable * _default, const char * _format, ... ) const;
		bool operator () ( PyObject *& _value, PyObject * _default, const char * _format, ... ) const;

	protected:
		const Eventable * m_self;

		EEventName m_event;
		PyObject * m_pyevent;		
	};
}

#	define EVENTABLE_CALL(Self, Event)\
	for( PyObject * EVENT_CALL_pyevent = Self->getEvent(Event); EVENT_CALL_pyevent != 0; EVENT_CALL_pyevent = 0 ) EventableCallOperator(Self, Event, EVENT_CALL_pyevent)

#	define EVENTABLE_ASK(Self, Event)\
	for( PyObject * EVENT_ASK_pyevent = Self->getEvent(Event); EVENT_ASK_pyevent != 0; EVENT_ASK_pyevent = 0 ) EventableAskOperator(Self, Event, EVENT_ASK_pyevent)