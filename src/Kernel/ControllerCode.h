#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef uint32_t ControllerId;
    //////////////////////////////////////////////////////////////////////////
    static MENGINE_CONSTEXPR ControllerId INVALID_CONTROLLER_ID = 0;
    //////////////////////////////////////////////////////////////////////////
    enum EControllerButton
    {
        CB_A,
        CB_B,
        CB_X,
        CB_Y,
        CB_LEFT_SHOULDER,
        CB_RIGHT_SHOULDER,
        CB_LEFT_TRIGGER,
        CB_RIGHT_TRIGGER,
        CB_MENU,
        CB_OPTIONS,
        CB_HOME,
        CB_LEFT_THUMB,
        CB_RIGHT_THUMB,
        CB_DPAD_UP,
        CB_DPAD_DOWN,
        CB_DPAD_LEFT,
        CB_DPAD_RIGHT,
        CB_TOUCHPAD,

        CONTROLLER_BUTTON_COUNT
    };
    //////////////////////////////////////////////////////////////////////////
    enum EControllerAxis
    {
        CA_LEFT_X,
        CA_LEFT_Y,
        CA_RIGHT_X,
        CA_RIGHT_Y,
        CA_LEFT_TRIGGER,
        CA_RIGHT_TRIGGER,

        CONTROLLER_AXIS_COUNT
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputControllerState
    {
        ControllerId controllerId;
        bool connected;
        bool buttons[CONTROLLER_BUTTON_COUNT];
        float axes[CONTROLLER_AXIS_COUNT];
    };
    //////////////////////////////////////////////////////////////////////////
}
