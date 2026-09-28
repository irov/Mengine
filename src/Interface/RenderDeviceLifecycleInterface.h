#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderDeviceLifecycleInterface
        : public Mixin
    {
    public:
        virtual void onRenderDeviceCreate() = 0;
        virtual void onRenderDeviceDestroy() = 0;
        virtual void onRenderDeviceLostPrepare() = 0;
        virtual void onRenderDeviceLostRestore() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
