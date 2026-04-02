#pragma once

#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/WheelCode.h"
#include "Kernel/TouchCode.h"

#include "Config/Timestamp.h"
#include "Config/Char.h"

#include "math/vec2.h"

#define MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE 32

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct InputBaseData
    {
        Timestamp timestamp;
    };
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
        InputBaseData base;
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
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        float pressure;

        WChar symbol;
        WChar text[MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE + 1];
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputAccelerometerEvent
    {
        InputBaseData base;
        InputSpecialData special;

        Timestamp timestampSensor;

        float dx;
        float dy;
        float dz;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseButtonEvent
    {
        InputBaseData base;
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
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        float pressure;

        EWheelCode wheel;
        int32_t scroll;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseMoveEvent
    {
        InputBaseData base;
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
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        ETouchCode touchId;

        float pressure;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseLeaveEvent
    {
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        ETouchCode touchId;

        float pressure;
    };
    //////////////////////////////////////////////////////////////////////////
}