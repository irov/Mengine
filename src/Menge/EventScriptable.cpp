#	include "EventScriptable.h"

#	include "ScriptEngine.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	EventScriptable::EventScriptable( PyObject * _callback )
		: m_callback( _callback )
	{		
		pybind::incref( _callback );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * EventScriptable::getCallback()
	{
		return m_callback;
	}
	//////////////////////////////////////////////////////////////////////////
	void EventScriptable::call( const char * _format, ... )
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
			m_result = new EventScriptable( _obj );
			m_valid = true;
		}

		PyObject * wrapp( Event * _event ) override
		{
			return static_cast<EventScriptable*>(_event)->getCallback();
		}
	}s_extract_Event_type;

}