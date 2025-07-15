#pragma once

#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/WheelCode.h"
#include "Kernel/TouchCode.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
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

        void pushMouseMoveEvent( ETouchCode _touchId, float _x, float _y, float _dx, float _dy, float _pressure, float _dpressure );
        void pushMouseButtonEvent( ETouchCode _touchId, float _x, float _y, EMouseButtonCode _button, float _pressure, bool _isDown );
        void pushKeyEvent( float _x, float _y, float _pressure, EKeyCode _code, bool _isDown, bool _repeating );
        void pushTextEvent( float _x, float _y, float _pressure, const WChar * _text );
        void pushMouseLeaveEvent( ETouchCode _touchId, float _x, float _y, float _pressure );
        void pushMouseEnterEvent( ETouchCode _touchId, float _x, float _y, float _pressure );
        void pushMouseWheelEvent( float _x, float _y, float _pressure, EWheelCode _wheel, int32_t _scroll );
        void pushAccelerometerEvent( float _dx, float _dy, float _dz );
    }
}