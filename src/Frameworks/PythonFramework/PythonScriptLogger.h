#pragma once

#include "Interface/LoggerInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/LoggerBase.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScriptLogger
        : public LoggerBase
    {
        DECLARE_FACTORABLE( PythonScriptLogger );

    public:
        PythonScriptLogger();
        ~PythonScriptLogger() override;

    public:
        PyObject * py_write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );

    public:
        void setSoftspace( int32_t _softspace );
        int32_t getSoftspace() const;

    protected:
        void log( const LoggerMessage & _loggerMessage ) override;

    protected:
        String m_messageCache;

        int32_t m_softspace;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScriptLogger, LoggerInterface> PythonScriptLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}