#pragma once

#include "../MCPCommandInterface.h"

#include "Interface/InputHandlerInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Map.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    class MCPInputContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPInputTouchCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPInputTouchCommand( MCPInputContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        struct TouchEvent
        {
            Timestamp timestamp;
            ETouchCode touchId;
            float x;
            float y;
            float pressure;
        };

        typedef void (MCPInputTouchCommand::*Action)( const TouchEvent & _event );
        typedef Map<ConstString, Action> MapActions;

        void move_( const TouchEvent & _event );
        void start_( const TouchEvent & _event );
        void end_( const TouchEvent & _event );

    protected:
        MCPInputContext * m_context;
        MapActions m_actions;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputTouchCommand, MCPCommandInterface> MCPInputTouchCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
