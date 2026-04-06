#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusiveLinked.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderResourceHandler
        : public Mixin
        , public IntrusiveLinked<MetalRenderResourceHandler>
    {
    public:
        MetalRenderResourceHandler();
        ~MetalRenderResourceHandler() override;

    public:
        void setMetalDevice( id<MTLDevice> _device );
        id<MTLDevice> getMetalDevice() const;

    public:
        virtual void onRenderReset() = 0;
        virtual bool onRenderRestore() = 0;

    protected:
        id<MTLDevice> m_device;
    };
}
