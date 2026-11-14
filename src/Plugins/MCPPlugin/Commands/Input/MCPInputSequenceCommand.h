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

    protected:
        MCPCommandHostInterface * m_host;
        MCPInputSequenceStepRegistry * m_registry;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputSequenceCommand, MCPCommandInterface> MCPInputSequenceCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
