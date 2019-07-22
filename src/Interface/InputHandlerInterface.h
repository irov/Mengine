#pragma once

#include "Interface/Interface.h"

#include "Kernel/KeyCode.h"
#include "Kernel/MouseCode.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum InputEventType
    {
        IET_KEY,
        IET_TEXT,
        IET_MOUSE_BUTTON,
        IET_MOUSE_WHELL,
        IET_MOUSE_MOVE,
        IET_MOUSE_POSITION,
        IET_MOUSE_ENTER,
        IET_MOUSE_LEAVE
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputKeyEvent
    {
        InputEventType type;

        float x;
        float y;
        KeyCode code;
        bool isDown;
        bool isRepeat;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputTextEvent
    {
        InputEventType type;

        float x;
        float y;
        WChar key;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseButtonEvent
    {
        InputEventType type;

        uint32_t touchId;
        float x;
        float y;
        MouseCode code;
        float pressure;
        bool isDown;
        bool isPressed;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseWheelEvent
    {
        InputEventType type;

        float x;
        float y;
        uint32_t code;
        int32_t wheel;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseMoveEvent
    {
        InputEventType type;

        uint32_t touchId;
        float x;
        float y;
        float dx;
        float dy;
        float pressure;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseEnterEvent
    {
        InputEventType type;

        uint32_t touchId;
        float x;
        float y;
        float pressure;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMouseLeaveEvent
    {
        InputEventType type;

        uint32_t touchId;
        float x;
        float y;
        float pressure;
    };
    //////////////////////////////////////////////////////////////////////////
    struct InputMousePositionEvent
    {
        InputEventType type;

        uint32_t touchId;
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
