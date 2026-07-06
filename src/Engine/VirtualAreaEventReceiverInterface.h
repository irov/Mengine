#pragma once

#include "Interface/EventationInterface.h"

#include "Kernel/RenderContext.h"
#include "Kernel/InputEvent.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EVirtualAreaEventFlag
    {
        EVENT_VIRTUAL_AREA_DRAG = 0,
        EVENT_VIRTUAL_AREA_DRAG_START,
        EVENT_VIRTUAL_AREA_DRAG_MOVE,
        EVENT_VIRTUAL_AREA_DRAG_END,
        EVENT_VIRTUAL_AREA_SCALE,
        EVENT_VIRTUAL_AREA_TOUCH,

        __EVENT_VIRTUAL_AREA_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class VirtualAreaEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onVirtualAreaDrag( const mt::vec2f & _position, const mt::vec2f & _percentage ) = 0;
        virtual void onVirtualAreaDragStart( const mt::vec2f & _position ) = 0;
        virtual void onVirtualAreaDragMove( const mt::vec2f & _position, const mt::vec2f & _velocity ) = 0;
        virtual void onVirtualAreaDragEnd( const mt::vec2f & _position, const mt::vec2f & _velocity ) = 0;
        virtual void onVirtualAreaScale( float _scaleFactor ) = 0;
        virtual void onVirtualAreaTouch( uint32_t _touchCount ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VirtualAreaEventReceiverInterface> VirtualAreaEventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( VirtualAreaEventReceiverInterface, EVENT_VIRTUAL_AREA_DRAG );
    EVENTATION_TYPEID( VirtualAreaEventReceiverInterface, EVENT_VIRTUAL_AREA_DRAG_START );
    EVENTATION_TYPEID( VirtualAreaEventReceiverInterface, EVENT_VIRTUAL_AREA_DRAG_MOVE );
    EVENTATION_TYPEID( VirtualAreaEventReceiverInterface, EVENT_VIRTUAL_AREA_DRAG_END );
    EVENTATION_TYPEID( VirtualAreaEventReceiverInterface, EVENT_VIRTUAL_AREA_SCALE );
    EVENTATION_TYPEID( VirtualAreaEventReceiverInterface, EVENT_VIRTUAL_AREA_TOUCH );
    //////////////////////////////////////////////////////////////////////////
}
