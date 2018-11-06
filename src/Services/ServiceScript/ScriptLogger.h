#pragma once

#include "Interface/LoggerInterface.h"

#include "pybind/types.hpp"

namespace Mengine
{
    class ScriptLogger
    {
    public:
        ScriptLogger();
        virtual ~ScriptLogger();

    public:
        virtual void write( const char * _msg, uint32_t _size );

    public:
        PyObject * py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );

    public:
        void setMessageLevel( EMessageLevel _level );
        EMessageLevel getMessageLevel() const;

        void setSoftspace( int _softspace );
        int getSoftspace() const;

    protected:
        EMessageLevel m_level;

        int m_softspace;
    };
}