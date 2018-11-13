#pragma once

#include "Interface/EventationInterface.h"
#include "Interface/InputHandlerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum HotSpotEventFlag
    {
        EVENT_KEY = 0,
        EVENT_TEXT,
        EVENT_MOUSE_BUTTON,
        EVENT_MOUSE_BUTTON_BEGIN,
        EVENT_MOUSE_BUTTON_END,
        EVENT_MOUSE_WHEEL,
        EVENT_MOUSE_MOVE,
        EVENT_MOUSE_ENTER,
        EVENT_MOUSE_LEAVE,
        EVENT_MOUSE_OVER_DESTROY,
        EVENT_ACTIVATE,
        EVENT_DEACTIVATE
    };
    //////////////////////////////////////////////////////////////////////////
    class HotSpotEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onHotSpotActivate() = 0;
        virtual void onHotSpotDeactivate() = 0;
        virtual void onHotSpotMouseLeave() = 0;
        virtual bool onHotSpotMouseEnter( float _x, float _y ) = 0;
        virtual bool onHotSpotKey( const InputKeyEvent & _event ) = 0;
        virtual bool onHotSpotText( const InputTextEvent & _event ) = 0;
        virtual bool onHotSpotMouseButton( const InputMouseButtonEvent & _event ) = 0;
        virtual bool onHotSpotMouseButtonBegin( const InputMouseButtonEvent & _event ) = 0;
        virtual bool onHotSpotMouseButtonEnd( const InputMouseButtonEvent & _event ) = 0;
        virtual bool onHotSpotMouseMove( const InputMouseMoveEvent & _event ) = 0;
        virtual bool onHotSpotMouseWheel( const InputMouseWheelEvent & _event ) = 0;
        virtual void onHotSpotMouseOverDestroy() = 0;
        virtual void onHotSpotMouseButtonBegin( uint32_t _enumerator, bool _isEnd ) = 0;
        virtual void onHotSpotMouseButtonEnd( uint32_t _enumerator, bool _isEnd ) = 0;
    };
}