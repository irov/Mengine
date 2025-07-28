#include "InputServiceHelper.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool isSpecialDown()
        {
            return INPUT_SERVICE()
                ->isSpecialDown();
        }
        //////////////////////////////////////////////////////////////////////////
        bool isAltDown()
        {
            return INPUT_SERVICE()
                ->isAltDown();
        }
        //////////////////////////////////////////////////////////////////////////
        bool isShiftDown()
        {
            return INPUT_SERVICE()
                ->isShiftDown();
        }
        //////////////////////////////////////////////////////////////////////////
        bool isControlDown()
        {
            return INPUT_SERVICE()
                ->isControlDown();
        }
        //////////////////////////////////////////////////////////////////////////
        bool isKeyDown( EKeyCode _keyCode )
        {
            return INPUT_SERVICE()
                ->isKeyDown( _keyCode );
        }
        //////////////////////////////////////////////////////////////////////////
        bool isExclusiveKeyDown( EKeyCode _keyCode )
        {
            return INPUT_SERVICE()
                ->isExclusiveKeyDown( _keyCode );
        }
        //////////////////////////////////////////////////////////////////////////
        bool isAnyKeyDown()
        {
            return INPUT_SERVICE()
                ->isAnyKeyDown();
        }
        //////////////////////////////////////////////////////////////////////////
        bool isAnyMouseButtonDown()
        {
            return INPUT_SERVICE()
                ->isAnyMouseButtonDown();
        }
        //////////////////////////////////////////////////////////////////////////
        bool isMouseButtonDown( EMouseButtonCode _button )
        {
            return INPUT_SERVICE()
                ->isMouseButtonDown( _button );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseMoveEvent( ETouchCode _touchId, float _x, float _y, float _dx, float _dy, float _pressure, float _dpressure )
        {
            float vx;
            float vy;
            if( INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy ) == false )
            {
                return;
            }

            InputMouseMoveEvent ev;

            INPUT_SERVICE()
                ->getSpecial( &ev.special );

            ev.touchId = _touchId;

            ev.position.screen = mt::vec2f( vx, vy );
            ev.position.world = mt::vec2f( 0.f, 0.f );
            ev.pressure = _pressure;

            ev.screenDelta = mt::vec2f( _dx, _dy );
            ev.worldDelta = mt::vec2f( 0.f, 0.f );
            ev.dpressure = _dpressure;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseButtonEvent( ETouchCode _touchId, float _x, float _y, EMouseButtonCode _button, float _pressure, bool _isDown )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputMouseButtonEvent ev;

            INPUT_SERVICE()
                ->getSpecial( &ev.special );

            ev.touchId = _touchId;

            ev.position.screen = mt::vec2f( vx, vy );
            ev.position.world = mt::vec2f( 0.f, 0.f );
            ev.pressure = _pressure;

            ev.button = _button;
            ev.isDown = _isDown;
            ev.isPressed = true;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushKeyEvent( float _x, float _y, float _pressure, EKeyCode _code, bool _isDown, bool _repeating )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputKeyEvent ev;

            INPUT_SERVICE()
                ->getSpecial( &ev.special );

            bool isAlreadyDown = INPUT_SERVICE()
                ->isKeyDown( _code );

            ev.position.screen = mt::vec2f( vx, vy );
            ev.position.world = mt::vec2f( 0.f, 0.f );
            ev.pressure = _pressure;

            ev.code = _code;
            ev.isDown = _isDown;
            ev.isRepeat = (isAlreadyDown == true && _isDown == true) || (_repeating == true);

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushTextEvent( float _x, float _y, float _pressure, const WChar * _text )
        {
            MENGINE_ASSERTION_FATAL( StdString::wcslen( _text ) >= 1, "text event must have at least one character" );

            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputTextEvent ev;

            INPUT_SERVICE()
                ->getSpecial( &ev.special );

            ev.position.screen = mt::vec2f( vx, vy );
            ev.position.world = mt::vec2f( 0.f, 0.f );
            ev.pressure = _pressure;
            
            ev.symbol = _text[0];

            StdString::wcscpy_safe( ev.text, _text, MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE );

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

            InputMouseLeaveEvent ev;

            INPUT_SERVICE()
                ->getSpecial( &ev.special );

            ev.touchId = _touchId;

            ev.position.screen = mt::vec2f( vx, vy );
            ev.position.world = mt::vec2f( 0.f, 0.f );
            ev.pressure = _pressure;

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

            InputMouseEnterEvent ev;

            INPUT_SERVICE()
                ->getSpecial( &ev.special );

            ev.touchId = _touchId;

            ev.position.screen = mt::vec2f( vx, vy );
            ev.position.world = mt::vec2f( 0.f, 0.f );
            ev.pressure = _pressure;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseWheelEvent( float _x, float _y, float _pressure, EWheelCode _wheel, int32_t _scroll )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputMouseWheelEvent ev;

            INPUT_SERVICE()
                ->getSpecial( &ev.special );

            ev.position.screen = mt::vec2f( vx, vy );
            ev.position.world = mt::vec2f( 0.f, 0.f );
            ev.pressure = _pressure;

            ev.wheel = _wheel;
            ev.scroll = _scroll;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushAccelerometerEvent( float _dx, float _dy, float _dz )
        {
            InputAccelerometerEvent ev;

            INPUT_SERVICE()
                ->getSpecial( &ev.special );

            ev.dx = _dx;
            ev.dy = _dy;
            ev.dz = _dz;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}