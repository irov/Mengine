#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class VirtualArea;
    //////////////////////////////////////////////////////////////////////////
    class VirtualAreaGlobalInputHandler
        : public InputHandlerInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( VirtualAreaGlobalInputHandler );

    public:
        explicit VirtualAreaGlobalInputHandler( VirtualArea * _area );
        ~VirtualAreaGlobalInputHandler() override;

    protected:
        void handleFocus( bool _focus ) override;
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;
        bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) override;
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        void handleMouseButtonEventCancel( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        void handleMouseMoveEnd( const InputMouseMoveEndEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    protected:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    protected:
        bool handleGestureMagnification( const InputGestureMagnificationEvent & _event ) override;
        bool handleGestureRotation( const InputGestureRotationEvent & _event ) override;
        bool handleGestureSmartMagnification( const InputGestureSmartMagnificationEvent & _event ) override;
        bool handleGestureSwipe( const InputGestureSwipeEvent & _event ) override;
        bool handleGestureDoubleTap( const InputGestureDoubleTapEvent & _event ) override;
        bool handleGesturePressure( const InputGesturePressureEvent & _event ) override;

    protected:
        VirtualArea * m_area;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VirtualAreaGlobalInputHandler, InputHandlerInterface> VirtualAreaGlobalInputHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
