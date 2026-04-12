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

    public:
        virtual void setCurrentDrawable( id<MTLDrawable> _drawable, id<MTLTexture> _drawableTexture ) = 0;
    };
}
