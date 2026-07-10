#pragma once

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

@interface MengineImGUIFramebufferDescriptor : NSObject<NSCopying>

@property(nonatomic, assign) NSUInteger sampleCount;
@property(nonatomic, assign) MTLPixelFormat colorPixelFormat;
@property(nonatomic, assign) MTLPixelFormat depthPixelFormat;
@property(nonatomic, assign) MTLPixelFormat stencilPixelFormat;

- (instancetype)initWithRenderPassDescriptor:(MTLRenderPassDescriptor *)_renderPassDescriptor;

@end
