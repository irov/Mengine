#	pragma once

#	include <map>
#	include <string>

#	include "pybind/pybind.hpp"

#	include "Config/Typedef.h"


namespace Menge
{
	class Eventable
	{
	public:
		Eventable();
		~Eventable();

	public:
		bool registerEvent( const String & _name, const String & _method, PyObject * _module );
		bool registerEvent( const String & _name, PyObject * _callback );
		PyObject * getEvent( const String & _name );

		void callEvent( const String & _name, const char * _format, ... );
		bool askEvent( bool & result, const String & _name, const char * _format, ... );

		void removeAllEvent();

	private:
		typedef std::map<String, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
}