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
        void pushMouseMoveEvent( ETouchCode _touchId, float _x, float _y, float _dx, float _dy, float _pressure );
        void pushMouseButtonEvent( ETouchCode _touchId, float _x, float _y, EMouseButtonCode _button, float _pressure, bool _isDown );
        void pushKeyEvent( float _x, float _y, EKeyCode _code, bool _isDown, bool _repeating );
        void pushTextEvent( float _x, float _y, const WChar * _key );
        void pushMouseLeaveEvent( ETouchCode _touchId, float _x, float _y, float _pressure );
        void pushMouseEnterEvent( ETouchCode _touchId, float _x, float _y, float _pressure );
        void pushMouseWheelEvent( float _x, float _y, EWheelCode _wheel, int32_t _scroll );
        void pushAccelerometerEvent( float _x, float _y, float _z );
    }
}