#	include "ScriptLogger.h"

#	include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptLogger::ScriptLogger( ServiceProviderInterface * _serviceProvider )
		: m_serviceProvider(_serviceProvider)
		, m_softspace(0)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ScriptLogger::~ScriptLogger()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptLogger::py_write( PyObject * _args, PyObject * _kwds )
	{
        (void)_kwds;

		if( pybind::tuple_check( _args ) == false )
		{
			return pybind::ret_none();
		}

        size_t tuple_size = pybind::tuple_size(_args);

        if( tuple_size == 0 )
        {
            pybind::ret_none();
        }
			
		PyObject * arg = pybind::tuple_getitem(_args, 0);

        if( pybind::string_check( arg ) == true )
        {
            size_t size;
            const char * str = pybind::string_to_char_and_size( arg, size );

            this->write( str, size );
        }
        else if( pybind::unicode_check( arg ) == true )
		{
            size_t size;
			const char * utf8 = pybind::unicode_to_utf8_and_size( arg, size );

			this->write( utf8, size );
		}

		return pybind::ret_none();
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::write( const char * _msg, size_t _size )
	{
		LOGGER_WARNING(m_serviceProvider).logMessage( _msg, _size );
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
	ScriptLoggerError::ScriptLoggerError( ServiceProviderInterface * _serviceProvider )
		: ScriptLogger(_serviceProvider)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLoggerError::write( const char * _msg, size_t _size )
	{
		LOGGER_ERROR(m_serviceProvider).logMessage( _msg, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptLoggerError::embedding()
	{
		pybind::interface_<ScriptLoggerError>("ErrorScriptLogger", true )
			.def_native("write", &ScriptLoggerError::py_write )
			.def_property("softspace", &ScriptLoggerError::getSoftspace, &ScriptLoggerError::setSoftspace )
			;

		PyObject * embedded = pybind::ptr(this);

		return embedded;
	}
}