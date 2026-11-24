#pragma once

#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/WheelCode.h"
#include "Kernel/TouchCode.h"
#include "Kernel/ControllerCode.h"

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
        bool isTouch;

        float pressure;

        EMouseButtonCode button;
        bool isDown;
        bool isPressed;
        bool isCancelled;
    };
    //////////////////////////////////////////////////////////////////////////
    enum EInputScrollUnit
    {
        ISU_STEPS,
        ISU_SCREEN,
        ISU_LINES,
        ISU_PAGES
    };
    //////////////////////////////////////////////////////////////////////////
    enum EInputGesturePhase : uint8_t
    {
        IGP_NONE,
        IGP_MAY_BEGIN,
        IGP_BEGIN,
        IGP_CHANGE,
        IGP_END,
        IGP_CANCEL
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputScrollData
    {
        int32_t steps;
        mt::vec2f delta;
        EInputScrollUnit unit;
        EInputGesturePhase phase;
        EInputGesturePhase momentumPhase;
        bool isPrecise;
        bool isDirectionInvertedFromDevice;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseWheelEvent
    {
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        float pressure;

        EWheelCode wheel;
        InputScrollData scroll;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseMoveEvent
    {
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        ETouchCode touchId;
        bool isTouch;

        float pressure;

        mt::vec2f screenDelta;
        mt::vec2f worldDelta;
        float dpressure;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseMoveEndEvent
    {
        InputBaseData base;
        InputSpecialData special;

        bool isTouch;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputPointerCancelEvent
    {
        InputBaseData base;
        InputSpecialData special;
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
    struct InputGestureMagnificationEvent
    {
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        float scale;
        EInputGesturePhase phase;
        bool isTouch;
        ETouchCode touchId0;
        ETouchCode touchId1;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputGestureRotationEvent
    {
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        float degrees;
        EInputGesturePhase phase;
        bool isTouch;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputGestureSmartMagnificationEvent
    {
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        bool isTouch;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputGestureSwipeEvent
    {
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        mt::vec2f direction;
        bool isTouch;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputGesturePressureEvent
    {
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        float pressure;
        int32_t stage;
        float stageTransition;
        EInputGesturePhase phase;
        bool isTouch;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputGestureDoubleTapEvent
    {
        InputBaseData base;
        InputSpecialData special;
        InputPositionData position;

        ETouchCode touchId;
        bool isTouch;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputControllerConnectEvent
    {
        InputBaseData base;

        ControllerId controllerId;
        bool connected;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputControllerButtonEvent
    {
        InputBaseData base;

        ControllerId controllerId;
        EControllerButton button;
        float value;
        bool isDown;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputControllerAxisEvent
    {
        InputBaseData base;

        ControllerId controllerId;
        EControllerAxis axis;
        float value;
    };
    //////////////////////////////////////////////////////////////////////////
}
