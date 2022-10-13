#pragma once

#include "Interface/LoggerInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/String.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class XlsScriptLogger
    {
    public:
        XlsScriptLogger( ELoggerLevel _level, uint32_t _color );
        ~XlsScriptLogger();

    public:
        PyObject * py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );
        PyObject * py_flush( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );

    public:
        void setSoftspace( int32_t _softspace );
        int32_t getSoftspace() const;

    protected:
        String m_messageCache;

        ELoggerLevel m_level;
        uint32_t m_color;

        int32_t m_softspace;
    };
}