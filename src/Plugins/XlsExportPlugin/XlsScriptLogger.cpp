#include "XlsScriptLogger.h"

#include "Interface/LoggerServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Logger.h"

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

        if( _kernel->string_check( arg ) == true )
        {
            size_t size;
            const Char * str = _kernel->string_to_char_and_size( arg, &size );

            this->write( str, size );
        }
        else if( _kernel->unicode_check( arg ) == true )
        {
            size_t size;
            const Char * utf8 = _kernel->unicode_to_utf8_and_size( arg, &size );

            this->write( utf8, size );
        }

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
    void XlsScriptLogger::write( const Char * _msg, size_t _size )
    {
        LOGGER_SERVICE()
            ->logMessage( m_level, 0, m_color, _msg, _size );
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