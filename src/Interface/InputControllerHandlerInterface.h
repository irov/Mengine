#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/InputEvent.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class InputControllerHandlerInterface
        : public Mixin
    {
    public:
        virtual void handleControllerConnectEvent( const InputControllerConnectEvent & _event ) = 0;
        virtual bool handleControllerButtonEvent( const InputControllerButtonEvent & _event ) = 0;
        virtual bool handleControllerAxisEvent( const InputControllerAxisEvent & _event ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<InputControllerHandlerInterface> InputControllerHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
