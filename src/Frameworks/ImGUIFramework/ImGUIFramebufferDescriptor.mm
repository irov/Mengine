#include "ImGUIFramebufferDescriptor.h"

//////////////////////////////////////////////////////////////////////////
@implementation MengineImGUIFramebufferDescriptor

- (instancetype)initWithRenderPassDescriptor:(MTLRenderPassDescriptor *)_renderPassDescriptor
{
    self = [super init];

    if( self == nil )
    {
        return nil;
    }

    MTLRenderPassColorAttachmentDescriptor * colorAttachment = _renderPassDescriptor.colorAttachments[0];
    self.sampleCount = colorAttachment.texture.sampleCount;
    self.colorPixelFormat = colorAttachment.texture.pixelFormat;
    self.depthPixelFormat = _renderPassDescriptor.depthAttachment.texture.pixelFormat;
    self.stencilPixelFormat = _renderPassDescriptor.stencilAttachment.texture.pixelFormat;

    return self;
}

- (nonnull id)copyWithZone:(nullable NSZone *)_zone
{
    MengineImGUIFramebufferDescriptor * copy = [[MengineImGUIFramebufferDescriptor allocWithZone:_zone] init];
    copy.sampleCount = self.sampleCount;
    copy.colorPixelFormat = self.colorPixelFormat;
    copy.depthPixelFormat = self.depthPixelFormat;
    copy.stencilPixelFormat = self.stencilPixelFormat;

    return copy;
}

- (NSUInteger)hash
{
    NSUInteger sampleCount = self.sampleCount & 0x3;
    NSUInteger colorFormat = self.colorPixelFormat & 0x3FF;
    NSUInteger depthFormat = self.depthPixelFormat & 0x3FF;
    NSUInteger stencilFormat = self.stencilPixelFormat & 0x3FF;

    return (stencilFormat << 22) | (depthFormat << 12) | (colorFormat << 2) | sampleCount;
}

- (BOOL)isEqual:(id)_object
{
    if( [_object isKindOfClass:[MengineImGUIFramebufferDescriptor class]] == NO )
    {
        return NO;
    }

    MengineImGUIFramebufferDescriptor * other = _object;

    return other.sampleCount == self.sampleCount
        && other.colorPixelFormat == self.colorPixelFormat
        && other.depthPixelFormat == self.depthPixelFormat
        && other.stencilPixelFormat == self.stencilPixelFormat;
}

@end
