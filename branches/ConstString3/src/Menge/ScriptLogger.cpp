#	include "ScriptLogger.h"

#	include "LogEngine.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptLogger::ScriptLogger( LogServiceInterface * _logService )
		: m_logService(_logService)
		, m_softspace(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::write( const String& _msg )
	{
		LOGGER_WARNING(m_logService).logMessage( _msg.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::setSoftspace( int _softspace )
	{
		m_softspace = _softspace;
	}
	//////////////////////////////////////////////////////////////////////////
	int ScriptLogger::getSoftspace() const
	{
		return m_softspace;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptLogger::embedding()
	{
		pybind::interface_<ScriptLogger>("ScriptLogger", true )
			.def("write", &ScriptLogger::write )
			.def_property("softspace", &ScriptLogger::getSoftspace, &ScriptLogger::setSoftspace )
			;

		PyObject * embedded = pybind::ptr(this);

		return embedded;
	}
	//////////////////////////////////////////////////////////////////////////
	ErrorScriptLogger::ErrorScriptLogger( LogServiceInterface * _logService )
		: ScriptLogger(_logService)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ErrorScriptLogger::write( const String& _msg )
	{
		LOGGER_ERROR(m_logService).logMessage( _msg.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ErrorScriptLogger::embedding()
	{
		pybind::interface_<ErrorScriptLogger>("ErrorScriptLogger", true )
			.def("write", &ErrorScriptLogger::write )
			.def_property("softspace", &ErrorScriptLogger::getSoftspace, &ErrorScriptLogger::setSoftspace )
			;

		PyObject * embedded = pybind::ptr(this);

		return embedded;
	}
}