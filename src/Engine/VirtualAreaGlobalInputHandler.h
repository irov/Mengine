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
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;
        bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) override;
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    protected:
        VirtualArea * m_area;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VirtualAreaGlobalInputHandler, InputHandlerInterface> VirtualAreaGlobalInputHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
