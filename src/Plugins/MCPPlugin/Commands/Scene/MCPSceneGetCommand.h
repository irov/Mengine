#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPSceneContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPSceneGetCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPSceneGetCommand( MCPSceneContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPSceneContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPSceneGetCommand, MCPCommandInterface> MCPSceneGetCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
