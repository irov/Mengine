#	pragma once

#	include <map>
#	include <string>

#	include "Config/Typedef.h"

#	include "EventEnum.h"

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class Scriptable;

	class Eventable
	{
	public:
		Eventable();
		~Eventable();

	public:
		bool registerEvent( EEventName _name, const String & _method, Scriptable * _scriptable );
		bool registerEvent( EEventName _name, const String & _method, PyObject * _module );
		

		bool registerEvent( EEventName _name, PyObject * _event );
		PyObject * getEvent( EEventName _name );

		void callEvent( EEventName _name, const char * _format, ... );
		bool askEvent( bool & result, EEventName _name, const char * _format, ... );

		void removeAllEvent();

	private:
		typedef std::map<EEventName, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
}
