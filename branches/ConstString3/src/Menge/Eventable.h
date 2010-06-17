#	pragma once

#	include <map>
#	include <string>

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

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
		bool registerEvent( EEventName _name, const ConstString & _method, PyObject * _module );
		PyObject * getEvent( EEventName _name );

		void callEvent( EEventName _name, const char * _format, ... );

		bool askEvent( bool & _result, EEventName _name, const char * _format, ... );
		bool askEvent( std::size_t & _result, EEventName _name, const char * _format, ... );

		void removeAllEvent();

	protected:
		PyObject * getEvent_( const ConstString & _method, PyObject * _module );

	private:
		typedef std::map<EEventName, PyObject *> TMapEvent;
		TMapEvent m_mapEvent;
	};
}
