#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPScriptSetCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPScriptSetCommand( MCPScriptContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPScriptContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptSetCommand, MCPCommandInterface> MCPScriptSetCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
