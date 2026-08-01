#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPDebuggerContext;
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPDebugScopesCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPDebugScopesCommand( MCPDebuggerContext * _debuggerContext, MCPScriptContext * _scriptContext );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
        bool availableWhilePaused( const jpp::object & _params ) const override;

    protected:
        MCPDebuggerContext * m_debuggerContext;
        MCPScriptContext * m_scriptContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPDebugScopesCommand, MCPCommandInterface> MCPDebugScopesCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
