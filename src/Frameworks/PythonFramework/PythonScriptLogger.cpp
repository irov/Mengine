#include "PythonScriptLogger.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Logger.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"

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
            return _kernel->ret_none();
        }

        PyObject * arg = _kernel->tuple_getitem( _args, 0 );

        size_t arg_str_size;
        const char * arg_str;

        if( _kernel->string_check( arg ) == true )
        {
            arg_str = _kernel->string_to_char_and_size( arg, &arg_str_size );            
        }
        else if( _kernel->unicode_check( arg ) == true )
        {
            arg_str = _kernel->unicode_to_utf8_and_size( arg, &arg_str_size );
        }
        else
        {
            return _kernel->ret_none();
        }

        if( MENGINE_STRCMP( arg_str, "\n" ) != 0 )
        {
            m_messageCache.append( arg_str, arg_str_size );

            return _kernel->ret_none();
        }

        ELoggerLevel level = this->getVerboseLevel();
        uint32_t color = this->getColor();

        if( HAS_OPTION( "tracescriptlog" ) == true )
        {
            Char function[256] = {'\0'};
            uint32_t lineno = 0;
            _kernel->get_traceback_function( function, 256, &lineno );

            LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "python" ), level, LFILTER_NONE, color, function, lineno, ELF_FLAG_NONE )( "%s", m_messageCache.c_str() );
        }
        else
        {
            LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "python" ), level, LFILTER_NONE, color, nullptr, 0, ELF_FLAG_NONE )( "%s", m_messageCache.c_str() );
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
    void PythonScriptLogger::log( const LoggerMessage & _loggerMessage )
    {
        MENGINE_UNUSED( _loggerMessage );

        //Empty;
    }
    //////////////////////////////////////////////////////////////////////////
}