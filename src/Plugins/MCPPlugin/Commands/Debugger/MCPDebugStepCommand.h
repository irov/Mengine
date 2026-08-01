#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPDebuggerContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPDebugStepCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPDebugStepCommand( MCPDebuggerContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
        bool availableWhilePaused( const jpp::object & _params ) const override;

    protected:
        MCPDebuggerContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPDebugStepCommand, MCPCommandInterface> MCPDebugStepCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
