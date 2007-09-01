#	pragma once

#	include "Event.h"

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class EventScriptable
		: public Event
	{
	public:
		EventScriptable( PyObject * _callback );

	public:
		PyObject * getCallback();

	public:
		void call( const char * _format, ... ) override;

	protected:
		PyObject * m_callback;
	};
}