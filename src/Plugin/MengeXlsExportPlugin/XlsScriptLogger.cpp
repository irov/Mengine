#	include "XlsScriptLogger.h"

#	include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XlsScriptLogger::XlsScriptLogger( EMessageLevel _level )
		: m_level(_level)
		, m_softspace(0)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    XlsScriptLogger::~XlsScriptLogger()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	PyObject * XlsScriptLogger::py_write( PyObject * _args, PyObject * _kwds )
	{
        (void)_kwds;

		if( pybind::tuple_check( _args ) == false )
		{
			return pybind::ret_none();
		}

        size_t tuple_size = pybind::tuple_size(_args);

        if( tuple_size == 0 )
        {
            return pybind::ret_none();
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
    PyObject * XlsScriptLogger::py_flush( PyObject * _args, PyObject * _kwds )
    {
        (void)_args;
        (void)_kwds;

        return pybind::ret_none();
    }
	//////////////////////////////////////////////////////////////////////////
	void XlsScriptLogger::write( const char * _msg, size_t _size )
	{
		LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(), m_level ).logMessage( _msg, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsScriptLogger::setSoftspace( int _softspace )
	{
		m_softspace = _softspace;
	}
	//////////////////////////////////////////////////////////////////////////
	int XlsScriptLogger::getSoftspace() const
	{
		return m_softspace;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * XlsScriptLogger::embedding( pybind::kernel_interface * _kernel, PyObject * _module )
	{
		pybind::interface_<XlsScriptLogger>( _kernel, "XlsScriptLogger", true, _module )
			.def_native("write", &XlsScriptLogger::py_write )
            .def_native("flush", &XlsScriptLogger::py_flush )
			.def_property("softspace", &XlsScriptLogger::getSoftspace, &XlsScriptLogger::setSoftspace )
			;

		PyObject * embedded = pybind::ptr( _kernel, this );

		return embedded;
	}
}