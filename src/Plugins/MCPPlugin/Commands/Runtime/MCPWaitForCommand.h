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
        void update() override;
        void cancel( uint32_t _requestId ) override;
        void disconnect() override;

    protected:
        struct PendingWait
        {
            uint32_t requestId = 0;
            uint64_t startGeneration = 0;
            MCPWaitConditionInterfacePtr condition;
            jpp::object params;
        };

        MCPCommandHostInterface * m_host;
        MCPWaitConditionRegistry * m_registry;
        PendingWait m_pendingWait;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPWaitForCommand, MCPCommandInterface> MCPWaitForCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
