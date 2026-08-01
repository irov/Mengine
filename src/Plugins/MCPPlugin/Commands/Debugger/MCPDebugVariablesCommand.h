#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPDebugVariablesCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPDebugVariablesCommand( MCPScriptContext * _scriptContext );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
        bool availableWhilePaused( const jpp::object & _params ) const override;

    protected:
        MCPScriptContext * m_scriptContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPDebugVariablesCommand, MCPCommandInterface> MCPDebugVariablesCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
