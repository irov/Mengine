#include "InputServiceHelper.h"

#include "Interface/InputServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void pushMouseMoveEvent( uint32_t _touchId, float _x, float _y, float _dx, float _dy, float _pressure )
        {
            float vx;
            float vy;
            if( INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy ) == false )
            {
                return;
            }

            InputUnionEvent ev;
            ev.move.type = IET_MOUSE_MOVE;

            ev.move.touchId = _touchId;
            ev.move.x = vx;
            ev.move.y = vy;
            ev.move.dx = _dx;
            ev.move.dy = _dy;
            ev.move.pressure = _pressure;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseButtonEvent( uint32_t _touchId, float _x, float _y, EMouseCode _code, float _pressure, bool _isDown )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;
            ev.button.type = IET_MOUSE_BUTTON;

            ev.button.touchId = _touchId;
            ev.button.x = vx;
            ev.button.y = vy;
            ev.button.code = _code;
            ev.button.pressure = _pressure;
            ev.button.isDown = _isDown;
            ev.button.isPressed = true;

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
            ev.key.type = IET_KEY;

            ev.key.x = vx;
            ev.key.y = vy;
            ev.key.code = _code;
            ev.key.isDown = _isDown;
            ev.key.isRepeat = _repeating;
            ev.key.isSpecial = false;

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
            ev.text.type = IET_TEXT;

            ev.text.x = vx;
            ev.text.y = vy;
            ev.text.key = _key;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseLeaveEvent( uint32_t _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;
            ev.leave.type = IET_MOUSE_LEAVE;

            ev.leave.touchId = _touchId;
            ev.leave.x = vx;
            ev.leave.y = vy;
            ev.leave.pressure = _pressure;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseEnterEvent( uint32_t _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;
            ev.enter.type = IET_MOUSE_ENTER;

            ev.enter.touchId = _touchId;
            ev.enter.x = vx;
            ev.enter.y = vy;
            ev.enter.pressure = _pressure;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseWheelEvent( float _x, float _y, EMouseCode _code, int32_t _whell )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;
            ev.wheel.type = IET_MOUSE_WHELL;

            ev.wheel.x = vx;
            ev.wheel.y = vy;
            ev.wheel.code = _code;
            ev.wheel.wheel = _whell;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMousePositionEvent( uint32_t _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            if( INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy ) == false )
            {
                return;
            }

            InputUnionEvent ev;
            ev.position.type = IET_MOUSE_POSITION;

            ev.position.touchId = _touchId;
            ev.position.x = vx;
            ev.position.y = vy;
            ev.position.pressure = _pressure;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
    }
}