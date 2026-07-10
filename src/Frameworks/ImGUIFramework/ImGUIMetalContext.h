#pragma once

#include "ImGUIFramebufferDescriptor.h"
#include "ImGUIMetalBuffer.h"

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

@interface MengineImGUIMetalContext : NSObject

@property(nonatomic, strong) id<MTLDevice> device;
@property(nonatomic, strong) id<MTLDepthStencilState> depthStencilState;
@property(nonatomic, strong) id<MTLSamplerState> samplerStateLinear;
@property(nonatomic, strong) id<MTLSamplerState> samplerStateNearest;
@property(nonatomic, strong) MengineImGUIFramebufferDescriptor * framebufferDescriptor;
@property(nonatomic, strong) NSMutableDictionary<MengineImGUIFramebufferDescriptor *, id<MTLRenderPipelineState>> * renderPipelineStateCache;
@property(nonatomic, strong) NSMutableArray<MengineImGUIMetalBuffer *> * bufferCache;
@property(nonatomic, strong) NSObject * bufferCacheLock;
@property(nonatomic, assign) double lastBufferCachePurge;

- (MengineImGUIMetalBuffer *)dequeueReusableBufferOfLength:(NSUInteger)_length device:(id<MTLDevice>)_device;
- (id<MTLRenderPipelineState>)renderPipelineStateForFramebufferDescriptor:(MengineImGUIFramebufferDescriptor *)_descriptor device:(id<MTLDevice>)_device;

@end
