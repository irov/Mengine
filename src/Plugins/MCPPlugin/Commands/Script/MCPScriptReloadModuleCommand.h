#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPScriptContext;
    class MCPDebuggerContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPScriptReloadModuleCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPScriptReloadModuleCommand( MCPScriptContext * _scriptContext, MCPDebuggerContext * _debuggerContext );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPScriptContext * m_scriptContext;
        MCPDebuggerContext * m_debuggerContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptReloadModuleCommand, MCPCommandInterface> MCPScriptReloadModuleCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
