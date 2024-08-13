#pragma once

#include "Interface/Interface.h"

#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/WheelCode.h"
#include "Kernel/TouchCode.h"

#include "Config/Char.h"

#define MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE 32

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EInputEventType
    {
        IET_KEY,
        IET_TEXT,
        IET_ACCELEROMETER,
        IET_MOUSE_BUTTON,
        IET_MOUSE_WHELL,
        IET_MOUSE_MOVE,
        IET_MOUSE_ENTER,
        IET_MOUSE_LEAVE
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
    class InputHandlerInterface
        : public Interface
    {
    public:
        virtual bool handleKeyEvent( const InputKeyEvent & _event ) = 0;
        virtual bool handleTextEvent( const InputTextEvent & _event ) = 0;

    public:
        virtual bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) = 0;

    public:
        virtual bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) = 0;
        virtual bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) = 0;
        virtual bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) = 0;
        virtual bool handleMouseMove( const InputMouseMoveEvent & _event ) = 0;
        virtual bool handleMouseWheel( const InputMouseWheelEvent & _event ) = 0;

    public:
        virtual bool handleMouseEnter( const InputMouseEnterEvent & _event ) = 0;
        virtual void handleMouseLeave( const InputMouseLeaveEvent & _event ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<InputHandlerInterface> InputHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}
