#pragma once

#include "Interface/Interface.h"

#include "Kernel/InputEvent.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class InputHandlerInterface
        : public Interface
    {
    public:
        virtual bool handleKeyEvent( const InputKeyEvent & _event ) = 0;
        virtual bool handleTextEvent( const InputTextEvent & _event ) = 0;

    public:
        virtual bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) = 0;

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
