#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MCPSceneContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPSceneFindCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPSceneFindCommand( MCPSceneContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        MCPSceneContext * m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPSceneFindCommand, MCPCommandInterface> MCPSceneFindCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
