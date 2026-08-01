#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPSceneContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPSceneSetCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPSceneSetCommand( MCPSceneContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPSceneContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPSceneSetCommand, MCPCommandInterface> MCPSceneSetCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
