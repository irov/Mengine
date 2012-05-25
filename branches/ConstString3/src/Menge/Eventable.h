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
		bool askEvent( bool & _result, EEventName _event, const char * _format, ... ) const;
		bool askEvent( size_t & _result, EEventName _event, const char * _format, ... ) const;
		bool askEvent( Scriptable *& _result, EEventName _event, const char * _format, ... ) const;

	public:
		void removeAllEvent();

	protected:
		PyObject * getEvent_( const char * _method, PyObject * _dict ) const;
		void removeEvent_( EEventName _event );

	private:
		typedef std::map<EEventName, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
}
