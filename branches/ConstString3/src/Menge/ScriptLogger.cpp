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
	PyObject * ScriptLogger::py_write( PyObject * _args, PyObject * _kwds )
	{
		if( pybind::tuple_check( _args ) == false )
		{
			return pybind::ret_none();
		}
			
		PyObject * arg = pybind::tuple_getitem(_args, 0);

		if( pybind::string_check( arg ) == true )
		{
			String msg;
			pybind::extract_value( arg, msg );

			this->write( msg );

			return pybind::ret_none();
		}
		
		if( pybind::unicode_check( arg ) == true )
		{
			size_t size;
			const char * utf8 = pybind::unicode_to_utf8( arg, size );

			this->write( utf8 );

			return pybind::ret_none();
		}

		return pybind::ret_none();
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
			.def_native("write", &ScriptLogger::py_write )
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
			.def_native("write", &ErrorScriptLogger::py_write )
			.def_property("softspace", &ErrorScriptLogger::getSoftspace, &ErrorScriptLogger::setSoftspace )
			;

		PyObject * embedded = pybind::ptr(this);

		return embedded;
	}
}