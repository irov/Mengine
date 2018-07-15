#include "ScriptLogger.h"

#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ScriptLogger::ScriptLogger()
		: m_level( LM_INFO )
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

        uint32_t tuple_size = pybind::tuple_size(_args);

        if( tuple_size == 0 )
        {
            pybind::ret_none();
        }
			
		PyObject * arg = pybind::tuple_getitem(_args, 0);

        if( pybind::string_check( arg ) == true )
        {
            uint32_t size;
            const char * str = pybind::string_to_char_and_size( arg, size );

            this->write( str, size );
        }
        else if( pybind::unicode_check( arg ) == true )
		{
            uint32_t size;
			const char * utf8 = pybind::unicode_to_utf8_and_size( arg, size );

			this->write( utf8, size );
		}

		return pybind::ret_none();
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::write( const char * _msg, uint32_t _size )
	{		
		LOGGER_VERBOSE_LEVEL( LOGGER_SERVICE(), m_level ).logMessage( _msg, _size );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptLogger::setMessageLevel( EMessageLevel _level )
	{
		m_level = _level;
	}
	//////////////////////////////////////////////////////////////////////////
	EMessageLevel ScriptLogger::getMessageLevel() const
	{
		return m_level;
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
}