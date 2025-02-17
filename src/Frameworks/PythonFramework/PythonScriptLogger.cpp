#include "PythonScriptLogger.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Logger.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"
#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScriptLogger::PythonScriptLogger()
        : m_color( LCOLOR_NONE )
        , m_softspace( 0 )
        , m_level( LM_SILENT )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptLogger::~PythonScriptLogger()
    {
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
    void PythonScriptLogger::setLoggerLevel( ELoggerLevel _level )
    {
        m_level = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    ELoggerLevel PythonScriptLogger::getLoggerLevel() const
    {
        return m_level;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PythonScriptLogger::write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds )
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

        if( StdString::strcmp( arg_str, "\n" ) != 0 )
        {
            StdString::strncat( m_message, arg_str, arg_str_size );

            return _kernel->ret_none();
        }

        uint32_t color = this->getColor();

        if( HAS_OPTION( "tracescriptlog" ) == true )
        {
            Path file = {'\0'};
            Path function = {'\0'};
            uint32_t lineno = 0;
            _kernel->get_statetrace_top( file, MENGINE_MAX_PATH, function, MENGINE_MAX_PATH, &lineno );

            LOGGER_VERBOSE_LEVEL( "python", m_level, LFILTER_NONE, color, file, lineno, function, LFLAG_SHORT | LFLAG_FILESTAMP | LFLAG_FUNCTIONSTAMP )("%s", m_message);
        }
        else
        {
            LOGGER_VERBOSE_LEVEL( "python", m_level, LFILTER_NONE, color, nullptr, 0, nullptr, LFLAG_SHORT )("%s", m_message);
        }

        StdString::strcpy_safe( m_message, "", MENGINE_LOGGER_MAX_MESSAGE );

        return _kernel->ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
}