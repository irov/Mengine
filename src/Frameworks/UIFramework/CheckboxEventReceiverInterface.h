#pragma once

#include "Interface/EventationInterface.h"
#include "Interface/InputHandlerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ECheckboxEventFlag
    {
        EVENT_CHECKBOX_MOUSE_ENTER = 0,
        EVENT_CHECKBOX_MOUSE_LEAVE,
        EVENT_CHECKBOX_MOUSE_PUSH,
        EVENT_CHECKBOX_MOUSE_PRESSED,
        EVENT_CHECKBOX_MOUSE_RELEASE,
        EVENT_CHECKBOX_CHANGE,

        __EVENT_BUTTON_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class CheckboxEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onCheckboxMouseEnter( bool _value ) = 0;
        virtual void onCheckboxMouseLeave( bool _value ) = 0;
        virtual void onCheckboxMousePush( bool _value ) = 0;
        virtual void onCheckboxMousePressed( bool _value ) = 0;
        virtual void onCheckboxMouseRelease( bool _value ) = 0;
        virtual void onCheckboxChange( bool _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CheckboxEventReceiverInterface> CheckboxEventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( CheckboxEventReceiverInterface, EVENT_CHECKBOX_MOUSE_ENTER );
    EVENTATION_TYPEID( CheckboxEventReceiverInterface, EVENT_CHECKBOX_MOUSE_LEAVE );
    EVENTATION_TYPEID( CheckboxEventReceiverInterface, EVENT_CHECKBOX_MOUSE_PUSH );
    EVENTATION_TYPEID( CheckboxEventReceiverInterface, EVENT_CHECKBOX_MOUSE_PRESSED );
    EVENTATION_TYPEID( CheckboxEventReceiverInterface, EVENT_CHECKBOX_MOUSE_RELEASE );
    EVENTATION_TYPEID( CheckboxEventReceiverInterface, EVENT_CHECKBOX_CHANGE );
    //////////////////////////////////////////////////////////////////////////
}