#include "ScriptLogger.h"

#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ScriptLogger::ScriptLogger()
        : m_level( LM_INFO )
        , m_softspace( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptLogger::~ScriptLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptLogger::py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds )
    {
        MENGINE_UNUSED( _kwds );

        if( _kernel->tuple_check( _args ) == false )
        {
            return _kernel->ret_none();
        }

        uint32_t tuple_size = _kernel->tuple_size( _args );

        if( tuple_size == 0 )
        {
            _kernel->ret_none();
        }

        PyObject * arg = _kernel->tuple_getitem( _args, 0 );

        if( _kernel->string_check( arg ) == true )
        {
            uint32_t size;
            const char * str = _kernel->string_to_char_and_size( arg, size );

            this->write( str, size );
        }
        else if( _kernel->unicode_check( arg ) == true )
        {
            uint32_t size;
            const char * utf8 = _kernel->unicode_to_utf8_and_size( arg, size );

            this->write( utf8, size );
        }

        return _kernel->ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptLogger::write( const Char * _msg, uint32_t _size )
    {
        LOGGER_VERBOSE_LEVEL( m_level, "Python", 0 ).logMessage( _msg, _size );
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
    void ScriptLogger::setSoftspace( int32_t _softspace )
    {
        m_softspace = _softspace;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t ScriptLogger::getSoftspace() const
    {
        return m_softspace;
    }
}