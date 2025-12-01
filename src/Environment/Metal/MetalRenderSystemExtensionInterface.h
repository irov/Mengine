#pragma once

#include "Interface/UnknownInterface.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

namespace Mengine
{
    class MetalRenderSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual void setMetalContext( id<MTLDevice> _device, CAMetalLayer * _layer ) = 0;
    };
}
