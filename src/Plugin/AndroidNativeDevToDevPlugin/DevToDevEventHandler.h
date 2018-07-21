#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    enum EnumDevToDevEventHandler
    {
        DEVTODEV_INITIALIZE,
    };

    class DevToDevEventHandler
        : public Mixin
    {
    public:
        virtual void onDevToDevInitialized() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDevEventHandler> DevToDevEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
