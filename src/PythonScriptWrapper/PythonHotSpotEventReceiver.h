#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Engine/HotSpotEventReceiverInterface.h"

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
        void onHotSpotMouseLeave() override;
        bool onHotSpotMouseEnter( float _x, float _y ) override;
        bool onHotSpotKey( const InputKeyEvent & _event ) override;
        bool onHotSpotText( const InputTextEvent & _event ) override;
        bool onHotSpotMouseButton( const InputMouseButtonEvent & _event ) override;
        bool onHotSpotMouseButtonBegin( const InputMouseButtonEvent & _event ) override;
        bool onHotSpotMouseButtonEnd( const InputMouseButtonEvent & _event ) override;
        bool onHotSpotMouseMove( const InputMouseMoveEvent & _event ) override;
        bool onHotSpotMouseWheel( const InputMouseWheelEvent & _event ) override;
        void onHotSpotMouseOverDestroy() override;
        void onHotSpotMouseButtonBegin( uint32_t _enumerator, bool _isEnd ) override;
        void onHotSpotMouseButtonEnd( uint32_t _enumerator, bool _isEnd ) override;
    };
}