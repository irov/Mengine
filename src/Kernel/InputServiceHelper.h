#pragma once

#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/WheelCode.h"
#include "Kernel/TouchCode.h"
#include "Kernel/ControllerCode.h"

#include "Config/Char.h"
#include "Config/Typedef.h"
#include "Config/Timestamp.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct InputSpecialData;
    struct InputScrollData;
    struct InputPositionData;
    struct RenderContext;
    enum EInputGesturePhase : uint8_t;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        bool isSpecialDown();
        bool isAltDown();
        bool isShiftDown();
        bool isControlDown();
        bool isKeyDown( EKeyCode _keyCode );
        bool isExclusiveKeyDown( EKeyCode _keyCode );
        bool isAnyKeyDown();
        bool isAnyMouseButtonDown();
        bool isMouseButtonDown( EMouseButtonCode _button );

        void calcInputPositionWorld( const RenderContext * _context, InputPositionData * const _position );
        void calcGlobalInputPositionWorld( InputPositionData * const _position );

        void pushMouseMoveEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, float _dx, float _dy, float _pressure, float _dpressure, bool _isTouch = false );
        void pushMouseMoveEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, float _dx, float _dy, float _pressure, float _dpressure, const InputSpecialData & _special, bool _isTouch = false );
        void pushTouchMoveEndEvent( Timestamp _timestamp );
        void pushMouseButtonEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, EMouseButtonCode _button, float _pressure, bool _isDown, bool _isCancelled = false, bool _isTouch = false );
        void pushMouseButtonEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, EMouseButtonCode _button, float _pressure, bool _isDown, const InputSpecialData & _special, bool _isCancelled = false, bool _isTouch = false );
        void pushKeyEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EKeyCode _code, bool _isDown, bool _repeating );
        void pushKeyEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EKeyCode _code, bool _isDown, bool _repeating, const InputSpecialData & _special );
        void pushTextEvent( Timestamp _timestamp, float _x, float _y, float _pressure, const WChar * _text );
        void pushMouseLeaveEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, float _pressure );
        void pushMouseEnterEvent( Timestamp _timestamp, ETouchCode _touchId, float _x, float _y, float _pressure );
        void pushMouseWheelEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EWheelCode _wheel, int32_t _scroll );
        void pushMouseWheelEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EWheelCode _wheel, int32_t _scroll, const InputSpecialData & _special );
        void pushMouseWheelEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EWheelCode _wheel, const InputScrollData & _scroll );
        void pushMouseWheelEvent( Timestamp _timestamp, float _x, float _y, float _pressure, EWheelCode _wheel, const InputScrollData & _scroll, const InputSpecialData & _special );
        void pushGestureMagnificationEvent( Timestamp _timestamp, float _x, float _y, float _scale, EInputGesturePhase _phase, const InputSpecialData & _special );
        void pushGestureRotationEvent( Timestamp _timestamp, float _x, float _y, float _degrees, EInputGesturePhase _phase, const InputSpecialData & _special );
        void pushGestureSmartMagnificationEvent( Timestamp _timestamp, float _x, float _y, const InputSpecialData & _special );
        void pushGestureSwipeEvent( Timestamp _timestamp, float _x, float _y, const mt::vec2f & _direction, const InputSpecialData & _special );
        void pushGesturePressureEvent( Timestamp _timestamp, float _x, float _y, float _pressure, int32_t _stage, float _stageTransition, EInputGesturePhase _phase, const InputSpecialData & _special );
        void pushAccelerometerEvent( Timestamp _timestamp, Timestamp _timestampSensor, float _dx, float _dy, float _dz );
        void pushControllerConnectEvent( Timestamp _timestamp, ControllerId _controllerId, bool _connected );
        void pushControllerButtonEvent( Timestamp _timestamp, ControllerId _controllerId, EControllerButton _button, float _value, bool _isDown );
        void pushControllerAxisEvent( Timestamp _timestamp, ControllerId _controllerId, EControllerAxis _axis, float _value );
    }
}
