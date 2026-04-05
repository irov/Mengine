#pragma once

#include "Interface/UnknownInterface.h"

#import <Metal/Metal.h>

namespace Mengine
{
    class MetalRenderImageExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual id<MTLTexture> getMetalTexture() const = 0;

    public:
        virtual void bind( uint32_t _stage ) = 0;
        virtual void unbind( uint32_t _stage ) = 0;
    };
}
