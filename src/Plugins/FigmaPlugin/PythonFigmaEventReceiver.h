#pragma once

#include "FigmaEventReceiverInterface.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonFigmaEventReceiver
        : public PythonEventReceiver
        , public FigmaEventReceiverInterface
        , public Factorable
    {
    public:
        PythonFigmaEventReceiver();
        ~PythonFigmaEventReceiver() override;

    public:
        void onFigmaTrigger( const FigmaTriggerEvent & _event ) override;
        void onFigmaAction( const FigmaActionEvent & _event, FigmaActionResponse * const _response ) override;
        void onFigmaFrameChanged( const String & _previousFrameId, const String & _currentFrameId ) override;
        void onFigmaOverlayOpened( const String & _frameId ) override;
        void onFigmaOverlayClosed( const String & _frameId ) override;
        void onFigmaStateChanged( const String & _sourceNodeId, const String & _previousStateId, const String & _currentStateId ) override;
    };
}
