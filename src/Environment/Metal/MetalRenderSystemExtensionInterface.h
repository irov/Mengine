#pragma once

#include "Interface/UnknownInterface.h"

#import <Metal/Metal.h>

namespace Mengine
{
    class MetalRenderSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual id<MTLDevice> getMetalDevice() const = 0;
        virtual id<MTLCommandBuffer> getMetalCommandBuffer() const = 0;
        virtual id<MTLRenderCommandEncoder> getMetalRenderCommandEncoder() const = 0;
        virtual MTLRenderPassDescriptor * getMetalRenderPassDescriptor() const = 0;

    public:
        virtual void setCurrentDrawable( id<MTLDrawable> _drawable, id<MTLTexture> _drawableTexture ) = 0;
    };
}
