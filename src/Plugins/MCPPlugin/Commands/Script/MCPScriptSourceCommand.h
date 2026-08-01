#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPScriptContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPScriptSourceCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPScriptSourceCommand( MCPScriptContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPScriptContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPScriptSourceCommand, MCPCommandInterface> MCPScriptSourceCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
