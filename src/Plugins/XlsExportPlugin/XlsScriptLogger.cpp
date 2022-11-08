#include "XlsScriptLogger.h"

#include "Interface/LoggerServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    XlsScriptLogger::XlsScriptLogger( ELoggerLevel _level, uint32_t _color )
        : m_level( _level )
        , m_color( _color )
        , m_softspace( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    XlsScriptLogger::~XlsScriptLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * XlsScriptLogger::py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds )
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

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "xls" ), m_level, LFILTER_NONE, m_color, nullptr, 0, ELF_FLAG_NONE ).operator()( "%s", m_messageCache.c_str() );
        
        m_messageCache.clear();

        return _kernel->ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * XlsScriptLogger::py_flush( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds )
    {
        MENGINE_UNUSED( _args );
        MENGINE_UNUSED( _kwds );

        return _kernel->ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsScriptLogger::setSoftspace( int32_t _softspace )
    {
        m_softspace = _softspace;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t XlsScriptLogger::getSoftspace() const
    {
        return m_softspace;
    }
    //////////////////////////////////////////////////////////////////////////
}