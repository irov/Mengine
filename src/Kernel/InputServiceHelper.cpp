#include "InputServiceHelper.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/ArrowServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "RenderContext.h"
#include "TimestampHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            template<class T>
            static void fillGestureEvent( Timestamp _timestamp, float _x, float _y, const InputSpecialData & _special, T * const _event )
            {
                float vx;
                float vy;
                INPUT_SERVICE()
                    ->validCursorPosition( _x, _y, &vx, &vy );

                _event->base.timestamp = _timestamp;
                _event->special = _special;
                _event->position.screen.set( vx, vy );
                _event->position.world.set( 0.f, 0.f );
            }
        }
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
        void calcInputPositionWorld( const RenderContext * _context, InputPositionData * const _position )
        {
            ARROW_SERVICE()
                ->calcMouseWorldPosition( _context, _position->screen, &_position->world );
        }
        //////////////////////////////////////////////////////////////////////////
        void calcGlobalInputPositionWorld( InputPositionData * const _position )
        {
            const RenderContext * context = PLAYER_SERVICE()
                ->getRenderContext();

            if( context->camera == nullptr )
            {
                return;
            }

            if( context->viewport == nullptr )
            {
                return;
            }

            if( context->resolution == nullptr )
            {
                return;
            }

            Helper::calcInputPositionWorld( context, _position );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseMoveEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, float _dx, float _dy, float _pressure, float _dpressure )
        {
            InputSpecialData special;
            INPUT_SERVICE()
                ->getSpecial( &special );

            Helper::pushMouseMoveEvent( _timestamp, _touchId, _x, _y, _dx, _dy, _pressure, _dpressure, special );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseMoveEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, float _dx, float _dy, float _pressure, float _dpressure, const InputSpecialData & _special )
        {
            float vx;
            float vy;
            if( INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy ) == false )
            {
                return;
            }

            InputMouseMoveEvent ev;
            ev.base.timestamp = _timestamp;

            ev.special = _special;

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
        void pushMouseButtonEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, EMouseButtonCode _button, float _pressure, bool _isDown, bool _isCancelled )
        {
            InputSpecialData special;
            INPUT_SERVICE()
                ->getSpecial( &special );

            Helper::pushMouseButtonEvent( _timestamp, _touchId, _x, _y, _button, _pressure, _isDown, special, _isCancelled );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseButtonEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, EMouseButtonCode _button, float _pressure, bool _isDown, const InputSpecialData & _special, bool _isCancelled )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputMouseButtonEvent ev;
            ev.base.timestamp = _timestamp;

            ev.special = _special;

            ev.touchId = _touchId;

            ev.position.screen = mt::vec2f( vx, vy );
            ev.position.world = mt::vec2f( 0.f, 0.f );
            ev.pressure = _pressure;

            ev.button = _button;
            ev.isDown = _isDown;
            ev.isPressed = true;
            ev.isCancelled = _isCancelled;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushKeyEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EKeyCode _code, bool _isDown, bool _repeating )
        {
            InputSpecialData special;
            INPUT_SERVICE()
                ->getSpecial( &special );

            Helper::pushKeyEvent( _timestamp, _x, _y, _pressure, _code, _isDown, _repeating, special );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushKeyEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EKeyCode _code, bool _isDown, bool _repeating, const InputSpecialData & _special )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputKeyEvent ev;
            ev.base.timestamp = _timestamp;

            ev.special = _special;

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
        void pushTextEvent( Timestamp _timestamp, float _x, float _y, float _pressure, const WChar * _text )
        {
            MENGINE_ASSERTION_FATAL( StdString::wcslen( _text ) >= 1, "text event must have at least one character" );

            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputTextEvent ev;
            ev.base.timestamp = _timestamp;

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
        void pushMouseLeaveEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputMouseLeaveEvent ev;
            ev.base.timestamp = _timestamp;

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
        void pushMouseEnterEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputMouseEnterEvent ev;
            ev.base.timestamp = _timestamp;

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
        void pushMouseWheelEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EWheelCode _wheel, int32_t _scroll )
        {
            InputSpecialData special;
            INPUT_SERVICE()
                ->getSpecial( &special );

            Helper::pushMouseWheelEvent( _timestamp, _x, _y, _pressure, _wheel, _scroll, special );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseWheelEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EWheelCode _wheel, int32_t _scroll, const InputSpecialData & _special )
        {
            InputScrollData scroll{};
            scroll.steps = _scroll;
            scroll.delta.set( 0.f, -(float)_scroll );

            Helper::pushMouseWheelEvent( _timestamp, _x, _y, _pressure, _wheel, scroll, _special );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseWheelEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EWheelCode _wheel, const InputScrollData & _scroll )
        {
            InputSpecialData special;
            INPUT_SERVICE()
                ->getSpecial( &special );

            Helper::pushMouseWheelEvent( _timestamp, _x, _y, _pressure, _wheel, _scroll, special );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushMouseWheelEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EWheelCode _wheel, const InputScrollData & _scroll, const InputSpecialData & _special )
        {
            float vx;
            float vy;
            INPUT_SERVICE()
                ->validCursorPosition( _x, _y, &vx, &vy );

            InputMouseWheelEvent ev;
            ev.base.timestamp = _timestamp;

            ev.special = _special;

            ev.position.screen = mt::vec2f( vx, vy );
            ev.position.world = mt::vec2f( 0.f, 0.f );
            ev.pressure = _pressure;

            ev.wheel = _wheel;
            ev.scroll = _scroll;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushGestureMagnificationEvent( Timestamp _timestamp, float _x, float _y, float _scale, EInputGesturePhase _phase, const InputSpecialData & _special )
        {
            InputGestureMagnificationEvent event;
            event.scale = _scale;
            event.phase = _phase;

            Detail::fillGestureEvent( _timestamp, _x, _y, _special, &event );

            INPUT_SERVICE()
                ->pushEvent( event );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushGestureRotationEvent( Timestamp _timestamp, float _x, float _y, float _degrees, EInputGesturePhase _phase, const InputSpecialData & _special )
        {
            InputGestureRotationEvent event;
            event.degrees = _degrees;
            event.phase = _phase;

            Detail::fillGestureEvent( _timestamp, _x, _y, _special, &event );

            INPUT_SERVICE()
                ->pushEvent( event );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushGestureSmartMagnificationEvent( Timestamp _timestamp, float _x, float _y, const InputSpecialData & _special )
        {
            InputGestureSmartMagnificationEvent event;

            Detail::fillGestureEvent( _timestamp, _x, _y, _special, &event );

            INPUT_SERVICE()
                ->pushEvent( event );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushGestureSwipeEvent( Timestamp _timestamp, float _x, float _y, const mt::vec2f & _direction, const InputSpecialData & _special )
        {
            InputGestureSwipeEvent event;
            event.direction = _direction;

            Detail::fillGestureEvent( _timestamp, _x, _y, _special, &event );

            INPUT_SERVICE()
                ->pushEvent( event );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushGesturePressureEvent( Timestamp _timestamp, float _x, float _y, float _pressure, int32_t _stage, float _stageTransition, EInputGesturePhase _phase, const InputSpecialData & _special )
        {
            InputGesturePressureEvent event;
            event.pressure = _pressure;
            event.stage = _stage;
            event.stageTransition = _stageTransition;
            event.phase = _phase;

            Detail::fillGestureEvent( _timestamp, _x, _y, _special, &event );

            INPUT_SERVICE()
                ->pushEvent( event );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushAccelerometerEvent( Timestamp _timestamp, Timestamp _timestampSensor, float _dx, float _dy, float _dz )
        {
            InputAccelerometerEvent ev;
            ev.base.timestamp = _timestamp;
            ev.timestampSensor = _timestampSensor;

            INPUT_SERVICE()
                ->getSpecial( &ev.special );

            ev.dx = _dx;
            ev.dy = _dy;
            ev.dz = _dz;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushControllerConnectEvent( Timestamp _timestamp, ControllerId _controllerId, bool _connected )
        {
            InputControllerConnectEvent ev;
            ev.base.timestamp = _timestamp;
            ev.controllerId = _controllerId;
            ev.connected = _connected;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushControllerButtonEvent( Timestamp _timestamp, ControllerId _controllerId, EControllerButton _button, float _value, bool _isDown )
        {
            InputControllerButtonEvent ev;
            ev.base.timestamp = _timestamp;
            ev.controllerId = _controllerId;
            ev.button = _button;
            ev.value = _value;
            ev.isDown = _isDown;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        void pushControllerAxisEvent( Timestamp _timestamp, ControllerId _controllerId, EControllerAxis _axis, float _value )
        {
            InputControllerAxisEvent ev;
            ev.base.timestamp = _timestamp;
            ev.controllerId = _controllerId;
            ev.axis = _axis;
            ev.value = _value;

            INPUT_SERVICE()
                ->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
