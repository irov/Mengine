#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPRuntimeContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPLogsReadCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPLogsReadCommand( MCPRuntimeContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPRuntimeContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPLogsReadCommand, MCPCommandInterface> MCPLogsReadCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
