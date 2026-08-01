#pragma once

#include "../MCPCommandInterface.h"
#include "Steps/MCPInputSequenceStepInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPInputSequenceCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPInputSequenceCommand( MCPCommandHostInterface * _host, MCPInputSequenceStepRegistry * _registry );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
        void update() override;
        void cancel( uint32_t _requestId ) override;
        void disconnect() override;

    protected:
        struct PendingSequence
        {
            uint32_t requestId = 0;
            uint64_t startGeneration = 0;
            uint64_t resumeGeneration = 0;
            Timestamp resumeTimestamp = 0;
            size_t index = 0;
            jpp::object steps;
        };

        MCPCommandHostInterface * m_host;
        MCPInputSequenceStepRegistry * m_registry;
        PendingSequence m_pendingSequence;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputSequenceCommand, MCPCommandInterface> MCPInputSequenceCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
