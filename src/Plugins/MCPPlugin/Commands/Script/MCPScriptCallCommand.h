#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPHandlerRegistry;
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPScriptCallCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPScriptCallCommand( MCPHandlerRegistry * _handlerRegistry, MCPScriptContext * _scriptContext );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPHandlerRegistry * m_handlerRegistry;
        MCPScriptContext * m_scriptContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptCallCommand, MCPCommandInterface> MCPScriptCallCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
