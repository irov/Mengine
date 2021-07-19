#include "InputServiceHelper.h"

#include "Interface/InputServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void pushMouseMoveEvent( ETouchCode _touchId, float _x, float _y, float _dx, float _dy, float _pressure )
        {
            float vx;
            float vy;
            if( INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy ) == false )
            {
                return;
            }

            InputUnionEvent ev;

            ev.type = IET_MOUSE_MOVE;

            INPUT_SERVICE()
                ->getSpecial( &ev.data.special );

            ev.data.move.touchId = _touchId;
            ev.data.move.x = vx;
            ev.data.move.y = vy;
            ev.data.move.dx = _dx;
            ev.data.move.dy = _dy;
            ev.data.move.pressure = _pressure;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseButtonEvent( ETouchCode _touchId, float _x, float _y, EMouseCode _code, float _pressure, bool _isDown )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;

            ev.type = IET_MOUSE_BUTTON;

            INPUT_SERVICE()
                ->getSpecial( &ev.data.special );

            ev.data.button.touchId = _touchId;
            ev.data.button.x = vx;
            ev.data.button.y = vy;
            ev.data.button.code = _code;
            ev.data.button.pressure = _pressure;
            ev.data.button.isDown = _isDown;
            ev.data.button.isPressed = true;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushKeyEvent( float _x, float _y, EKeyCode _code, bool _isDown, bool _repeating )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;

            ev.type = IET_KEY;

            INPUT_SERVICE()
                ->getSpecial( &ev.data.special );

            ev.data.key.x = vx;
            ev.data.key.y = vy;
            ev.data.key.code = _code;
            ev.data.key.isDown = _isDown;
            ev.data.key.isRepeat = _repeating;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushTextEvent( float _x, float _y, WChar _key )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;

            ev.type = IET_TEXT;

            INPUT_SERVICE()
                ->getSpecial( &ev.data.special );

            ev.data.text.x = vx;
            ev.data.text.y = vy;
            ev.data.text.key = _key;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseLeaveEvent( ETouchCode _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;

            ev.type = IET_MOUSE_LEAVE;

            INPUT_SERVICE()
                ->getSpecial( &ev.data.special );

            ev.data.leave.touchId = _touchId;
            ev.data.leave.x = vx;
            ev.data.leave.y = vy;
            ev.data.leave.pressure = _pressure;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseEnterEvent( ETouchCode _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;

            ev.type = IET_MOUSE_ENTER;

            INPUT_SERVICE()
                ->getSpecial( &ev.data.special );

            ev.data.enter.touchId = _touchId;
            ev.data.enter.x = vx;
            ev.data.enter.y = vy;
            ev.data.enter.pressure = _pressure;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseWheelEvent( float _x, float _y, EWheelCode _code, int32_t _scroll )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;

            ev.type = IET_MOUSE_WHELL;

            INPUT_SERVICE()
                ->getSpecial( &ev.data.special );

            ev.data.wheel.x = vx;
            ev.data.wheel.y = vy;
            ev.data.wheel.code = _code;
            ev.data.wheel.scroll = _scroll;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}