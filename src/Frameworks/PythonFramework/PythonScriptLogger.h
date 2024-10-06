#pragma once

#include "Interface/LoggerInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScriptLogger
        : public Factorable
    {
        DECLARE_FACTORABLE( PythonScriptLogger );

    public:
        PythonScriptLogger();
        ~PythonScriptLogger() override;

    public:
        void setSoftspace( int32_t _softspace );
        int32_t getSoftspace() const;

    public:
        void setColor( uint32_t _color );
        uint32_t getColor() const;

    public:
        void setLoggerLevel( ELoggerLevel _level );
        ELoggerLevel getLoggerLevel() const;

    public:
        PyObject * write( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds );

    protected:
        uint32_t m_color;
        int32_t m_softspace;
        ELoggerLevel m_level;

        Char m_message[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScriptLogger> PythonScriptLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}