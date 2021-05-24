#pragma once

#include "Interface/EventationInterface.h"
#include "Interface/InputHandlerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EButtonEventFlag
    {
        EVENT_BUTTON_MOUSE_ENTER = 0,
        EVENT_BUTTON_MOUSE_LEAVE,
        EVENT_BUTTON_MOUSE_PUSH,
        EVENT_BUTTON_MOUSE_PRESSED,
        EVENT_BUTTON_MOUSE_RELEASE,
        EVENT_BUTTON_MOUSE_BUTTON,
        EVENT_BUTTON_MOUSE_BUTTON_BEGIN,
        EVENT_BUTTON_MOUSE_BUTTON_END,

        __EVENT_BUTTON_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class ButtonEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onButtonMouseEnter() = 0;
        virtual void onButtonMouseLeave() = 0;
        virtual void onButtonMousePush() = 0;
        virtual void onButtonMousePressed() = 0;
        virtual void onButtonMouseRelease() = 0;
        virtual void onButtonMouseButton() = 0;
        virtual void onButtonMouseButtonBegin() = 0;
        virtual void onButtonMouseButtonEnd() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ButtonEventReceiverInterface> ButtonEventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( ButtonEventReceiverInterface, EVENT_BUTTON_MOUSE_ENTER );
    EVENTATION_TYPEID( ButtonEventReceiverInterface, EVENT_BUTTON_MOUSE_LEAVE );
    EVENTATION_TYPEID( ButtonEventReceiverInterface, EVENT_BUTTON_MOUSE_PUSH );
    EVENTATION_TYPEID( ButtonEventReceiverInterface, EVENT_BUTTON_MOUSE_PRESSED );
    EVENTATION_TYPEID( ButtonEventReceiverInterface, EVENT_BUTTON_MOUSE_RELEASE );
    EVENTATION_TYPEID( ButtonEventReceiverInterface, EVENT_BUTTON_MOUSE_BUTTON );
    EVENTATION_TYPEID( ButtonEventReceiverInterface, EVENT_BUTTON_MOUSE_BUTTON_BEGIN );
    EVENTATION_TYPEID( ButtonEventReceiverInterface, EVENT_BUTTON_MOUSE_BUTTON_END );
    //////////////////////////////////////////////////////////////////////////
}