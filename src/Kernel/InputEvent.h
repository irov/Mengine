#pragma once

#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/WheelCode.h"
#include "Kernel/TouchCode.h"

#include "Config/Char.h"

#define MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE 32

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct InputSpecialData
    {
        bool isAlt;
        bool isShift;
        bool isControl;
        bool isSpecial;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputKeyEvent
    {
        InputSpecialData special;

        float x;
        float y;
        float pressure;

        EKeyCode code;
        bool isDown;
        bool isRepeat;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputTextEvent
    {
        InputSpecialData special;

        float x;
        float y;
        float pressure;

        WChar symbol;
        WChar text[MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE];
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputAccelerometerEvent
    {
        InputSpecialData special;

        float dx;
        float dy;
        float dz;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseButtonEvent
    {
        InputSpecialData special;

        ETouchCode touchId;

        float x;
        float y;
        float pressure;

        EMouseButtonCode button;
        bool isDown;
        bool isPressed;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseWheelEvent
    {
        InputSpecialData special;

        float x;
        float y;
        float pressure;

        EWheelCode wheel;
        int32_t scroll;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseMoveEvent
    {
        InputSpecialData special;

        ETouchCode touchId;

        float x;
        float y;
        float pressure;

        float dx;
        float dy;
        float dpressure;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseEnterEvent
    {
        InputSpecialData special;

        ETouchCode touchId;

        float x;
        float y;
        float pressure;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseLeaveEvent
    {
        InputSpecialData special;

        ETouchCode touchId;

        float x;
        float y;
        float pressure;
    };
    //////////////////////////////////////////////////////////////////////////
}