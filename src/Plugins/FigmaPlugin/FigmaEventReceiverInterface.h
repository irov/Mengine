#pragma once

#include "Interface/EventReceiverInterface.h"

#include "FigmaInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum FigmaEventFlag
    {
        EVENT_FIGMA_TRIGGER = 0,
        EVENT_FIGMA_ACTION,
        EVENT_FIGMA_FRAME_CHANGED,
        EVENT_FIGMA_OVERLAY_OPENED,
        EVENT_FIGMA_OVERLAY_CLOSED,
        EVENT_FIGMA_STATE_CHANGED,

        __EVENT_FIGMA_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class FigmaEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onFigmaTrigger( const FigmaTriggerEvent & _event ) = 0;
        virtual void onFigmaAction( const FigmaActionEvent & _event, FigmaActionResponse * const _response ) = 0;
        virtual void onFigmaFrameChanged( const String & _previousFrameId, const String & _currentFrameId ) = 0;
        virtual void onFigmaOverlayOpened( const String & _frameId ) = 0;
        virtual void onFigmaOverlayClosed( const String & _frameId ) = 0;
        virtual void onFigmaStateChanged( const String & _sourceNodeId, const String & _previousStateId, const String & _currentStateId ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( FigmaEventReceiverInterface, EVENT_FIGMA_TRIGGER );
    EVENTATION_TYPEID( FigmaEventReceiverInterface, EVENT_FIGMA_ACTION );
    EVENTATION_TYPEID( FigmaEventReceiverInterface, EVENT_FIGMA_FRAME_CHANGED );
    EVENTATION_TYPEID( FigmaEventReceiverInterface, EVENT_FIGMA_OVERLAY_OPENED );
    EVENTATION_TYPEID( FigmaEventReceiverInterface, EVENT_FIGMA_OVERLAY_CLOSED );
    EVENTATION_TYPEID( FigmaEventReceiverInterface, EVENT_FIGMA_STATE_CHANGED );
    //////////////////////////////////////////////////////////////////////////
}
