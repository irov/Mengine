#pragma once

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct MetalRenderFrameContext
    {
        id<MTLCommandBuffer> commandBuffer;
        id<MTLRenderCommandEncoder> renderEncoder;
        MTLRenderPassDescriptor * renderPassDescriptor;
        id<MTLTexture> drawableTexture;
        id<MTLTexture> depthStencilTexture;
    };
    //////////////////////////////////////////////////////////////////////////
}
