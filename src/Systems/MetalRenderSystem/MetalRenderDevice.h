#pragma once

#include "Interface/RenderDeviceInterface.h"

#include "Environment/Metal/MetalRenderDeviceExtensionInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderDevice
        : public RenderDeviceInterface
        , public MetalRenderDeviceExtensionInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderDevice );
        DECLARE_UNKNOWABLE();

    public:
        MetalRenderDevice();
        ~MetalRenderDevice() override;

    public:
        bool initialize( id<MTLDevice> _device );
        void finalize();

    public:
        id<MTLDevice> getMetalDevice() const override;

    protected:
        id<MTLDevice> m_device;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderDevice, RenderDeviceInterface> MetalRenderDevicePtr;
    //////////////////////////////////////////////////////////////////////////
}
