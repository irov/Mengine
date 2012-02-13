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

		void callEvent( EEventName _event, const char * _format, ... );
		void callEventDeferred( EEventName _event, const char * _format, ... );

		bool askEvent( bool & _result, EEventName _event, const char * _format, ... );
		bool askEvent( size_t & _result, EEventName _event, const char * _format, ... );
		bool askEvent( Scriptable *& _result, EEventName _event, const char * _format, ... );

		void removeAllEvent();

	protected:
		PyObject * getEventFromDict_( const char * _method, PyObject * _dict );
		void removeEvent_( EEventName _event );

	private:
		typedef std::map<EEventName, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
}
