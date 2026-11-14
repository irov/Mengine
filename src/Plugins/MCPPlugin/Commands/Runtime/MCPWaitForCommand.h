#pragma once

#include "../MCPCommandInterface.h"
#include "../Wait/MCPWaitConditionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPWaitForCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPWaitForCommand( MCPCommandHostInterface * _host, MCPWaitConditionRegistry * _registry );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPCommandHostInterface * m_host;
        MCPWaitConditionRegistry * m_registry;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPWaitForCommand, MCPCommandInterface> MCPWaitForCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
