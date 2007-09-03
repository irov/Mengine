#	include "Event.h"

#	include "ScriptEngine.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Event::Event( PyObject * _callback )
		: m_callback( _callback )
	{		
		pybind::incref( m_callback );
	}
	//////////////////////////////////////////////////////////////////////////
	Event::~Event()
	{
		pybind::decref( m_callback );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Event::getCallback()
	{
		return m_callback;
	}
	//////////////////////////////////////////////////////////////////////////
	void Event::call( const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		Holder<ScriptEngine>::hostage()
			->callFunction( m_callback, _format, valist );

		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	static struct extract_Event_type
		: public pybind::type_cast_result<Event *>
	{
		void apply( PyObject * _obj ) override
		{
			m_result = new Event( _obj );
			m_valid = true;
		}

		PyObject * wrapp( Event * _event ) override
		{
			return _event->getCallback();
		}
	}s_extract_Event_type;

}