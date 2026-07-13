#pragma once

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct MetalRenderFrameContext
    {
        uint64_t frameId;
        id<MTLCommandBuffer> commandBuffer;
        id<MTLRenderCommandEncoder> renderEncoder;
        MTLRenderPassDescriptor * renderPassDescriptor;
        id<MTLTexture> drawableTexture;
        id<MTLTexture> depthStencilTexture;
    };
    //////////////////////////////////////////////////////////////////////////
}
