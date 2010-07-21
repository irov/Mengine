#	include "ScriptEngine.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptLogger::ScriptLogger()
		: m_softspace(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::write( const String& _msg )
	{
		//MENGE_LOG_INFO( _msg.c_str() );
		Logger::get()->logMessage( _msg, LM_LOG );
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
		pybind::class_<ScriptLogger>("ScriptLogger", true )
			.def("write", &ScriptLogger::write )
			.def_property("softspace", &ScriptLogger::getSoftspace, &ScriptLogger::setSoftspace )
			;

		PyObject * embedded = pybind::ptr(this);

		return embedded;
	}
	//////////////////////////////////////////////////////////////////////////
	void ErrorScriptLogger::write( const String& _msg )
	{
		Logger::get()->logMessage( _msg, LM_ERROR );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ErrorScriptLogger::embedding()
	{
		pybind::class_<ErrorScriptLogger>("ErrorScriptLogger", true )
			.def("write", &ErrorScriptLogger::write )
			.def_property("softspace", &ErrorScriptLogger::getSoftspace, &ErrorScriptLogger::setSoftspace )
			;

		PyObject * embedded = pybind::ptr(this);

		return embedded;
	}
}