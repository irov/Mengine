#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Engine/VirtualAreaEventReceiverInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonVirtualAreaEventReceiver
        : public PythonEventReceiver
        , public VirtualAreaEventReceiverInterface
        , public Factorable
    {
    public:
        PythonVirtualAreaEventReceiver();
        ~PythonVirtualAreaEventReceiver() override;

    public:
        void onVirtualAreaDrag( const mt::vec2f & _position, const mt::vec2f & _percentage ) override;
        void onVirtualAreaDragStart( const mt::vec2f & _position ) override;
        void onVirtualAreaDragMove( const mt::vec2f & _position, const mt::vec2f & _velocity ) override;
        void onVirtualAreaDragEnd( const mt::vec2f & _position, const mt::vec2f & _velocity ) override;
        void onVirtualAreaScale( float _scaleFactor ) override;
        void onVirtualAreaTouch( uint32_t _touchCount ) override;
    };
}
