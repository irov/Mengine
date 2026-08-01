#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPScriptModulesCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptModulesCommand, MCPCommandInterface> MCPScriptModulesCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
