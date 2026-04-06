#include "iOSMetalView.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)

@implementation iOSMetalView
{
    id<MTLDevice> _device;
    id<MTLCommandQueue> _commandQueue;
    id<MTLCommandBuffer> _commandBuffer;
    id<CAMetalDrawable> _currentDrawable;
    id<MTLRenderCommandEncoder> _renderEncoder;
    MTLRenderPassDescriptor * _renderPassDescriptor;
}

+ (Class)layerClass
{
    return [CAMetalLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame device:(id<MTLDevice>)device
{
    self = [super initWithFrame:frame];

    if( self != nil )
    {
        _device = device;
        _commandQueue = [device newCommandQueue];
        _commandBuffer = nil;
        _currentDrawable = nil;
        _renderEncoder = nil;
        _renderPassDescriptor = nil;

        CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;
        metalLayer.device = _device;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        metalLayer.framebufferOnly = YES;
        metalLayer.opaque = YES;

        self.contentScaleFactor = [UIScreen mainScreen].scale;
        self.multipleTouchEnabled = YES;
    }

    return self;
}

- (BOOL)beginRender
{
    if( _device == nil || _commandQueue == nil )
    {
        return NO;
    }

    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;
    _currentDrawable = [metalLayer nextDrawable];

    if( _currentDrawable == nil )
    {
        return NO;
    }

    _commandBuffer = [_commandQueue commandBuffer];

    if( _commandBuffer == nil )
    {
        _currentDrawable = nil;

        return NO;
    }

    _renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    _renderPassDescriptor.colorAttachments[0].texture = _currentDrawable.texture;
    _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    _renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    _renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake( 0.0, 0.0, 0.0, 1.0 );

    _renderEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:_renderPassDescriptor];

    if( _renderEncoder == nil )
    {
        _currentDrawable = nil;
        _commandBuffer = nil;
        _renderPassDescriptor = nil;

        return NO;
    }

    return YES;
}

- (BOOL)endRender
{
    if( _renderEncoder == nil || _commandBuffer == nil || _currentDrawable == nil )
    {
        return NO;
    }

    [_renderEncoder endEncoding];
    [_commandBuffer presentDrawable:_currentDrawable];
    [_commandBuffer commit];

    _renderEncoder = nil;
    _commandBuffer = nil;
    _currentDrawable = nil;
    _renderPassDescriptor = nil;

    return YES;
}

- (NSInteger)drawableWidth
{
    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;

    return (NSInteger)(metalLayer.drawableSize.width);
}

- (NSInteger)drawableHeight
{
    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;

    return (NSInteger)(metalLayer.drawableSize.height);
}

- (id<MTLDevice>)metalDevice
{
    return _device;
}

- (id<CAMetalDrawable>)currentDrawable
{
    return _currentDrawable;
}

- (MTLRenderPassDescriptor *)currentRenderPassDescriptor
{
    return _renderPassDescriptor;
}

- (id<MTLCommandBuffer>)currentCommandBuffer
{
    return _commandBuffer;
}

- (id<MTLRenderCommandEncoder>)currentRenderEncoder
{
    return _renderEncoder;
}

- (void)layoutSubviews
{
    [super layoutSubviews];

    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;
    CGFloat scale = self.contentScaleFactor;
    CGSize boundsSize = self.bounds.size;

    metalLayer.drawableSize = CGSizeMake( boundsSize.width * scale, boundsSize.height * scale );
}

@end

#endif
