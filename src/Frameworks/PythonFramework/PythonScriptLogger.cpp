#include "PythonScriptLogger.h"

#include "Interface/OptionsServiceInterface.h"

#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScriptLogger::PythonScriptLogger()
        : m_softspace( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptLogger::~PythonScriptLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PythonScriptLogger::py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds )
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
            size_t size;
            const char * str = _kernel->string_to_char_and_size( arg, &size );

            m_messageCache.append( str, size );
        }
        else if( _kernel->unicode_check( arg ) == true )
        {
            size_t size;
            const char * utf8 = _kernel->unicode_to_utf8_and_size( arg, &size );

            m_messageCache.append( utf8, size );
        }

        if( m_messageCache.find( '\n' ) == String::npos )
        {
            return _kernel->ret_none();
        }

        ELoggerLevel level = this->getVerboseLevel();
        uint32_t color = this->getColor();

        if( HAS_OPTION( "tracescriptlog" ) == true )
        {
            Char function[256] = {'\0'};
            uint32_t lineno = 0;
            _kernel->get_traceback_function( function, 256, &lineno );

            LOGGER_VERBOSE_LEVEL( ConstString::none(), level, color, function, lineno ).setNewline( false ).operator()( "%s", m_messageCache.c_str() );
        }
        else
        {
            LOGGER_VERBOSE_LEVEL( ConstString::none(), level, color, nullptr, 0 ).setNewline( false ).operator()( "%s", m_messageCache.c_str() );
        }

        m_messageCache.clear();

        return _kernel->ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptLogger::setSoftspace( int32_t _softspace )
    {
        m_softspace = _softspace;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t PythonScriptLogger::getSoftspace() const
    {
        return m_softspace;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _color );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        //Empty;
    }
    //////////////////////////////////////////////////////////////////////////
}