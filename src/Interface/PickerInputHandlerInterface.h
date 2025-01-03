#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/RenderContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PickerInputHandlerInterface
        : public Mixin
    {
    public:
        virtual bool handleKeyEvent( const RenderContext * _context, const InputKeyEvent & _event ) = 0;
        virtual bool handleTextEvent( const RenderContext * _context, const InputTextEvent & _event ) = 0;

    public:
        virtual bool handleMouseButtonEvent( const RenderContext * _context, const InputMouseButtonEvent & _event ) = 0;
        virtual bool handleMouseButtonEventBegin( const RenderContext * _context, const InputMouseButtonEvent & _event ) = 0;
        virtual bool handleMouseButtonEventEnd( const RenderContext * _context, const InputMouseButtonEvent & _event ) = 0;
        virtual bool handleMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event ) = 0;
        virtual bool handleMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event ) = 0;

    public:
        virtual bool handleMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event ) = 0;
        virtual void handleMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PickerInputHandlerInterface> PickerInputHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}
