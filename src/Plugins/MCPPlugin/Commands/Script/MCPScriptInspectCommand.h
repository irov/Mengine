#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPScriptInspectCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPScriptInspectCommand( MCPScriptContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPScriptContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptInspectCommand, MCPCommandInterface> MCPScriptInspectCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
