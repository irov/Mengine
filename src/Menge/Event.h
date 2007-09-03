#	pragma once

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}


namespace Menge
{
	class Event
	{
	public:
		Event( PyObject * _callback );
		virtual ~Event();

	public:
		virtual void call( const char * _format, ... );

	public:
		PyObject * getCallback();

	protected:
		PyObject * m_callback;
	};
}