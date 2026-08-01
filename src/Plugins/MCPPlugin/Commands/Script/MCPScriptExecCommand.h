#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPDebuggerContext;
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPScriptExecCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPScriptExecCommand( MCPScriptContext * _scriptContext, MCPDebuggerContext * _debuggerContext );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
        bool availableWhilePaused( const jpp::object & _params ) const override;

    protected:
        MCPScriptContext * m_scriptContext;
        MCPDebuggerContext * m_debuggerContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptExecCommand, MCPCommandInterface> MCPScriptExecCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
