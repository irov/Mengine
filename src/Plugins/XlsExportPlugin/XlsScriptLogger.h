#pragma once

#include "Interface/LoggerInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class XlsScriptLogger
    {
    public:
        XlsScriptLogger( ELoggerLevel _level, uint32_t _color );
        virtual ~XlsScriptLogger();

    public:
        virtual void write( const Char * _msg, size_t _size );

    public:
        PyObject * py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );
        PyObject * py_flush( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );

    public:
        void setSoftspace( int32_t _softspace );
        int32_t getSoftspace() const;

    protected:
        ELoggerLevel m_level;
        uint32_t m_color;

        int32_t m_softspace;
    };
}