#pragma once

#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/WheelCode.h"
#include "Kernel/TouchCode.h"

#include "Config/Char.h"

#include "math/vec2.h"

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
    struct InputPositionData
    {
        mt::vec2f screen;
        mt::vec2f world;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputKeyEvent
    {
        InputSpecialData special;
        InputPositionData position;

        float pressure;

        EKeyCode code;
        bool isDown;
        bool isRepeat;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputTextEvent
    {
        InputSpecialData special;
        InputPositionData position;

        float pressure;

        WChar symbol;
        WChar text[MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE + 1];
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
        InputPositionData position;

        ETouchCode touchId;

        float pressure;

        EMouseButtonCode button;
        bool isDown;
        bool isPressed;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseWheelEvent
    {
        InputSpecialData special;
        InputPositionData position;

        float pressure;

        EWheelCode wheel;
        int32_t scroll;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseMoveEvent
    {
        InputSpecialData special;
        InputPositionData position;

        ETouchCode touchId;

        float pressure;

        mt::vec2f screenDelta;
        mt::vec2f worldDelta;
        float dpressure;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseEnterEvent
    {
        InputSpecialData special;
        InputPositionData position;

        ETouchCode touchId;

        float pressure;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseLeaveEvent
    {
        InputSpecialData special;
        InputPositionData position;

        ETouchCode touchId;

        float pressure;
    };
    //////////////////////////////////////////////////////////////////////////
}