#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class MCPInputContext;
    class MCPSceneContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPInputTouchCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPInputTouchCommand( MCPInputContext * _context, MCPSceneContext * _sceneContext );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        enum ETouchAction
        {
            ETA_MOVE,
            ETA_START,
            ETA_END,
            ETA_CANCEL
        };

        typedef Map<ConstString, ETouchAction> MapActions;

    protected:
        MCPInputContext * m_context;
        MCPSceneContext * m_sceneContext;
        MapActions m_actions;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputTouchCommand, MCPCommandInterface> MCPInputTouchCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
