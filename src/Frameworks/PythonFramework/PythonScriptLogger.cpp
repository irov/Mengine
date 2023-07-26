#include "PythonScriptLogger.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Logger.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScriptLogger::PythonScriptLogger()
        : m_color( LCOLOR_NONE )
        , m_softspace( 0 )
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
        const Char * arg_str;

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
            MENGINE_STRNCAT( m_message, arg_str, arg_str_size );

            return _kernel->ret_none();
        }

        uint32_t color = this->getColor();

        if( HAS_OPTION( "tracescriptlog" ) == true )
        {
            Char function[MENGINE_MAX_PATH] = {'\0'};
            uint32_t lineno = 0;
            _kernel->get_traceback_function( function, MENGINE_MAX_PATH, &lineno );

            LOGGER_VERBOSE_LEVEL( "python", LM_WARNING, LFILTER_NONE, color, function, lineno, LFLAG_SHORT | LFLAG_FUNCTIONSTAMP )("%s", m_message);
        }
        else
        {
            LOGGER_VERBOSE_LEVEL( "python", LM_WARNING, LFILTER_NONE, color, nullptr, 0, LFLAG_SHORT )("%s", m_message);
        }

        MENGINE_STRCPY( m_message, "" );

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
    void PythonScriptLogger::setColor( uint32_t _color )
    {
        m_color = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t PythonScriptLogger::getColor() const
    {
        return m_color;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptLogger::_log( const LoggerMessage & _loggerMessage )
    {
        MENGINE_UNUSED( _loggerMessage );

        //Empty;
    }
    //////////////////////////////////////////////////////////////////////////
}