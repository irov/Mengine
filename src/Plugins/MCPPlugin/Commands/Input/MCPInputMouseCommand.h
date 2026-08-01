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
    class MCPInputMouseCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        explicit MCPInputMouseCommand( MCPInputContext * _context );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        struct MouseEvent
        {
            Timestamp timestamp;
            float x;
            float y;
            float dx;
            float dy;
            EMouseButtonCode button;
            int32_t wheel;
        };

        typedef void (MCPInputMouseCommand::*Action)( const MouseEvent & _event );
        typedef Map<ConstString, Action> MapActions;

        void move_( const MouseEvent & _event );
        void down_( const MouseEvent & _event );
        void up_( const MouseEvent & _event );
        void click_( const MouseEvent & _event );
        void doubleClick_( const MouseEvent & _event );
        void wheel_( const MouseEvent & _event );

    protected:
        MCPInputContext * m_context;
        MapActions m_actions;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputMouseCommand, MCPCommandInterface> MCPInputMouseCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
