#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    class MCPService;

    //////////////////////////////////////////////////////////////////////////
    class MCPLogger
        : public LoggerBase
    {
        DECLARE_FACTORABLE( MCPLogger );

    public:
        MCPLogger();
        ~MCPLogger() override;

    public:
        void setService( MCPService * _service );

    protected:
        void _log( const LoggerRecordInterfacePtr & _record ) override;

    protected:
        MCPService * m_service;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPLogger, LoggerInterface> MCPLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
