#pragma once

#include "Kernel/HotSpotEventReceiverInterface.h"

namespace Mengine
{    
    class BaseHotSpotEventReceiver
        : public HotSpotEventReceiverInterface
    {
    public:
        void onHotSpotActivate() override;
        void onHotSpotDeactivate() override;
        bool onHotSpotMouseEnter( const InputMouseEnterEvent & _event ) override;
        void onHotSpotMouseLeave( const InputMouseLeaveEvent & _event ) override;
        bool onHotSpotKey( const InputKeyEvent & _event ) override;
        bool onHotSpotText( const InputTextEvent & _event ) override;
        bool onHotSpotAccelerometer( const InputAccelerometerEvent & _event ) override;
        bool onHotSpotMouseButton( const InputMouseButtonEvent & _event ) override;
        bool onHotSpotMouseButtonBegin( const InputMouseButtonEvent & _event ) override;
        bool onHotSpotMouseButtonEnd( const InputMouseButtonEvent & _event ) override;
        bool onHotSpotMouseMove( const InputMouseMoveEvent & _event ) override;
        bool onHotSpotMouseWheel( const InputMouseWheelEvent & _event ) override;
        void onHotSpotMouseOverDestroy() override;
        void onHotSpotMouseButtonBegin( UniqueId _enumerator, bool _isEnd ) override;
        void onHotSpotMouseButtonEnd( UniqueId _enumerator, bool _isEnd ) override;
    };
}