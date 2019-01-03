#pragma once

#include "Interface/LoggerInterface.h"

#include "Kernel/Factorable.h"

#include "pybind/types.hpp"

namespace Mengine
{
    class ScriptLogger
        : public Factorable
    {
    public:
        ScriptLogger();
        ~ScriptLogger() override;

    public:
        virtual void write( const Char * _msg, uint32_t _size );

    public:
        PyObject * py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );

    public:
        void setMessageLevel( EMessageLevel _level );
        EMessageLevel getMessageLevel() const;

        void setSoftspace( int _softspace );
        int getSoftspace() const;

    protected:
        EMessageLevel m_level;

        int32_t m_softspace;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptLogger> ScriptLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}