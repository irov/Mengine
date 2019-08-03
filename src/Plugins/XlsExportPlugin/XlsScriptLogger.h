#pragma once

#include "Interface/LoggerInterface.h"

#include "Config/Typedef.h"

#include "pybind/types.hpp"
#include "pybind/kernel.hpp"

namespace Mengine
{
    class XlsScriptLogger
    {
    public:
        XlsScriptLogger( ELoggerLevel _level );
        virtual ~XlsScriptLogger();

    public:
        virtual void write( const Char * _msg, uint32_t _size );

    public:
        PyObject * py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );
        PyObject * py_flush( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );

    public:
        void setSoftspace( int32_t _softspace );
        int32_t getSoftspace() const;

    protected:
        ELoggerLevel m_level;

        int32_t m_softspace;
    };
}