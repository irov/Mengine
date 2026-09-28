#pragma once

#include "Interface/RenderDeviceLifecycleInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderDeviceLifecycleable
        : public Mixin
    {
    public:
        RenderDeviceLifecycleable();
        ~RenderDeviceLifecycleable() override;

    public:
        virtual RenderDeviceLifecycleInterface * getRenderDeviceLifecycleable();
        virtual const RenderDeviceLifecycleInterface * getRenderDeviceLifecycleable() const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderDeviceLifecycleable> RenderDeviceLifecycleablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_RENDER_DEVICE_LIFECYCLEABLE()\
public:\
    Mengine::RenderDeviceLifecycleInterface * getRenderDeviceLifecycleable() override { return this; }\
    const Mengine::RenderDeviceLifecycleInterface * getRenderDeviceLifecycleable() const override { return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
