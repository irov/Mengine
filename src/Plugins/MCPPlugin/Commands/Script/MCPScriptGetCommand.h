#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPScriptGetCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPScriptGetCommand( MCPScriptContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPScriptContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptGetCommand, MCPCommandInterface> MCPScriptGetCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
