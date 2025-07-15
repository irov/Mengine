#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/HotSpotEventReceiverInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonHotSpotEventReceiver
        : public HotSpotEventReceiverInterface
        , public PythonEventReceiver
        , public Factorable
    {
    public:
        PythonHotSpotEventReceiver();
        ~PythonHotSpotEventReceiver() override;

    public:
        void onHotSpotActivate() override;
        void onHotSpotDeactivate() override;
        bool onHotSpotMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event ) override;
        void onHotSpotMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event ) override;
        bool onHotSpotKey( const RenderContext * _context, const InputKeyEvent & _event ) override;
        bool onHotSpotText( const RenderContext * _context, const InputTextEvent & _event ) override;
        bool onHotSpotMouseButton( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool onHotSpotMouseButtonBegin( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool onHotSpotMouseButtonEnd( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool onHotSpotMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event ) override;
        bool onHotSpotMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event ) override;
        void onHotSpotMouseOverDestroy() override;
    };
}