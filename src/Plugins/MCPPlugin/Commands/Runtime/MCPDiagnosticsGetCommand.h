#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPRuntimeContext;
    class MCPSceneContext;
    class MCPResourceContext;
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPDiagnosticsGetCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPDiagnosticsGetCommand( MCPCommandHostInterface * _host, MCPRuntimeContext * _runtimeContext, MCPSceneContext * _sceneContext, MCPResourceContext * _resourceContext, MCPScriptContext * _scriptContext );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPCommandHostInterface * m_host;
        MCPRuntimeContext * m_runtimeContext;
        MCPSceneContext * m_sceneContext;
        MCPResourceContext * m_resourceContext;
        MCPScriptContext * m_scriptContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPDiagnosticsGetCommand, MCPCommandInterface> MCPDiagnosticsGetCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
