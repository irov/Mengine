#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPResourceContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPResourceRevertCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPResourceRevertCommand( MCPResourceContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPResourceContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPResourceRevertCommand, MCPCommandInterface> MCPResourceRevertCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
