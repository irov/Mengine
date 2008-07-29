#	pragma once

#	include <map>
#	include <string>

#	include "pybind/pybind.hpp"

#	include "Config/Typedef.h"

#	include "EventEnum.h"

namespace Menge
{
	class Eventable
	{
	public:
		Eventable();
		~Eventable();

	public:
		bool registerEvent( EEventName _name, const String & _method, PyObject * _module );
		bool registerEvent( EEventName _name, PyObject * _callback );
		PyObject * getEvent( EEventName _name );

		void callEvent( EEventName _name, const char * _format, ... );
		bool askEvent( bool & result, EEventName _name, const char * _format, ... );

		void removeAllEvent();

	private:
		typedef std::map<EEventName, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
}