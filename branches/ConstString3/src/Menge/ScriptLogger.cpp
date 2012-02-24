#	include "ScriptLogger.h"

#	include "LogEngine.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptLogger::ScriptLogger( LogSystemInterface * _logSystem )
		: m_logSystem(_logSystem)
		, m_softspace(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::write( const String& _msg )
	{
		LOGGER_INFO(m_logSystem)( _msg.c_str() );
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
	ErrorScriptLogger::ErrorScriptLogger( LogSystemInterface * _logSystem )
		: ScriptLogger(_logSystem)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ErrorScriptLogger::write( const String& _msg )
	{
		LOGGER_ERROR(m_logSystem)( _msg.c_str() );
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