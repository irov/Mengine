#pragma once

#include "MCPHandlerInterface.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "MCPService" )

    public:
        virtual bool run( const String & _host, const String & _port, const String & _token, const String & _mode ) = 0;
        virtual void stop() = 0;

    public:
        virtual bool addHandler( const ConstString & _name, const MCPHandlerInterfacePtr & _handler ) = 0;
        virtual void removeHandler( const MCPHandlerInterfacePtr & _handler ) = 0;

    public:
        virtual bool isConnected() const = 0;
        virtual bool isDebuggerPaused() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define MCP_SERVICE()\
    ((Mengine::MCPServiceInterface *)SERVICE_GET(Mengine::MCPServiceInterface))
//////////////////////////////////////////////////////////////////////////
