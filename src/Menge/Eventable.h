#	pragma once

#	include "Node.h"

#	include <map>
#	include <string>

class TiXmlElement;

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class Eventable
		: public virtual Node
	{
	public:
		Eventable();
		~Eventable();

	public:
		bool registerEvent( const std::string &_name, PyObject * _module, const std::string & _method  );
		bool registerEvent( const std::string &_name, PyObject * _callback );
		PyObject * getEvent( const std::string &_name );

		void callEvent( const std::string &_name, const char * _format, ... );
		bool askEvent( bool & result, const std::string &_name, const char * _format, ... );

		void removeAllEvent();
	public:
		void loader( TiXmlElement * _xml );

	private:
		typedef std::map<std::string, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
}