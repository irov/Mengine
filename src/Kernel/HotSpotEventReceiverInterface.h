#pragma once

#include "Interface/EventationInterface.h"
#include "Interface/PickerInputHandlerInterface.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EHotSpotEventFlag
    {
        EVENT_HOTSPOT_KEY = 0,
        EVENT_HOTSPOT_TEXT,
        EVENT_HOTSPOT_MOUSE_BUTTON,
        EVENT_HOTSPOT_MOUSE_BUTTON_BEGIN,
        EVENT_HOTSPOT_MOUSE_BUTTON_END,
        EVENT_HOTSPOT_MOUSE_WHEEL,
        EVENT_HOTSPOT_MOUSE_MOVE,
        EVENT_HOTSPOT_MOUSE_ENTER,
        EVENT_HOTSPOT_MOUSE_LEAVE,
        EVENT_HOTSPOT_MOUSE_OVER_DESTROY,
        EVENT_HOTSPOT_ACTIVATE,
        EVENT_HOTSPOT_DEACTIVATE,

        __EVENT_HOTSPOT_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class HotSpotEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onHotSpotActivate() = 0;
        virtual void onHotSpotDeactivate() = 0;
        virtual bool onHotSpotMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event ) = 0;
        virtual void onHotSpotMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event ) = 0;
        virtual bool onHotSpotKey( const RenderContext * _context, const InputKeyEvent & _event ) = 0;
        virtual bool onHotSpotText( const RenderContext * _context, const InputTextEvent & _event ) = 0;
        virtual bool onHotSpotMouseButton( const RenderContext * _context, const InputMouseButtonEvent & _event ) = 0;
        virtual bool onHotSpotMouseButtonBegin( const RenderContext * _context, const InputMouseButtonEvent & _event ) = 0;
        virtual bool onHotSpotMouseButtonEnd( const RenderContext * _context, const InputMouseButtonEvent & _event ) = 0;
        virtual bool onHotSpotMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event ) = 0;
        virtual bool onHotSpotMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event ) = 0;
        virtual void onHotSpotMouseOverDestroy() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HotSpotEventReceiverInterface> HotSpotEventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_KEY );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_TEXT );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_MOUSE_BUTTON );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_MOUSE_BUTTON_BEGIN );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_MOUSE_BUTTON_END );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_MOUSE_WHEEL );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_MOUSE_MOVE );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_MOUSE_ENTER );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_MOUSE_LEAVE );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_MOUSE_OVER_DESTROY );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_ACTIVATE );
    EVENTATION_TYPEID( HotSpotEventReceiverInterface, EVENT_HOTSPOT_DEACTIVATE );
    //////////////////////////////////////////////////////////////////////////
}