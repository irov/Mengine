#	pragma once

#	include <map>
#	include <string>

#	include "pybind/pybind.hpp"

namespace Menge
{
	class Eventable
	{
	public:
		Eventable();
		~Eventable();

	public:
		bool registerEvent( const std::string & _name, const std::string & _method, PyObject * _module );
		bool registerEvent( const std::string & _name, PyObject * _callback );
		PyObject * getEvent( const std::string & _name );

		void callEvent( const std::string & _name, const char * _format, ... );
		bool askEvent( bool & result, const std::string & _name, const char * _format, ... );

		void removeAllEvent();

	private:
		typedef std::map<std::string, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
}