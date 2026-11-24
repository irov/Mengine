#pragma once

#include "Interface/InputHandlerInterface.h"

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Map.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    class MCPInputContext;
    class MCPSceneContext;

    //////////////////////////////////////////////////////////////////////////
    class MCPInputMouseCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPInputMouseCommand( MCPInputContext * _context, MCPSceneContext * _sceneContext );

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;

    protected:
        struct MouseEventDesc
        {
            InputSpecialData special;
            Timestamp timestamp;
            float x;
            float y;
            float dx;
            float dy;
            EMouseButtonCode button;
            int32_t wheel;
            InputScrollData scroll;
            float scale = 1.f;
            float degrees = 0.f;
            mt::vec2f swipeDirection = mt::vec2f( 0.f, 0.f );
            float pressure = 0.f;
            int32_t pressureStage = 0;
            float stageTransition = 0.f;
            EInputGesturePhase gesturePhase = IGP_CHANGE;
        };

        typedef void (MCPInputMouseCommand::*Action)( const MouseEventDesc & _event );
        typedef Map<ConstString, Action> MapActions;

        void move_( const MouseEventDesc & _event );
        void down_( const MouseEventDesc & _event );
        void up_( const MouseEventDesc & _event );
        void click_( const MouseEventDesc & _event );
        void doubleClick_( const MouseEventDesc & _event );
        void wheel_( const MouseEventDesc & _event );
        void scroll_( const MouseEventDesc & _event );
        void pinch_( const MouseEventDesc & _event );
        void rotate_( const MouseEventDesc & _event );
        void smartMagnify_( const MouseEventDesc & _event );
        void swipe_( const MouseEventDesc & _event );
        void pressure_( const MouseEventDesc & _event );

    protected:
        MCPInputContext * m_context;
        MCPSceneContext * m_sceneContext;
        MapActions m_actions;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputMouseCommand, MCPCommandInterface> MCPInputMouseCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
