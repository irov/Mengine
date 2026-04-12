#include "iOSMetalView.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)

@implementation iOSMetalView
{
    id<MTLDevice> _device;
    id<CAMetalDrawable> _currentDrawable;
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
        _currentDrawable = nil;

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
    if( _device == nil )
    {
        return NO;
    }

    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;
    _currentDrawable = [metalLayer nextDrawable];

    if( _currentDrawable == nil )
    {
        return NO;
    }

    return YES;
}

- (BOOL)endRender
{
    _currentDrawable = nil;

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
